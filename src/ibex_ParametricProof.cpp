/* ============================================================================
 * I B E X - Newton-based Parametric proofs
 * ============================================================================
 * Copyright   : Ecole des Mines de Nantes and CNRS
 *
 * License     : This program can be distributed under the terms of the GNU LGPL.
 *               See the file COPYING.LESSER.
 *
 * Author(s)   : Gilles Chabert, Alexandre Goldsztejn
 * Created     : Sep 06, 2016
 * ---------------------------------------------------------------------------- */

#include "ibex_ParametricProof.h"
#include "ibex_Cont.h"

#include "ibex_Newton.h"
#include "ibex_Linear.h"
#include "ibex_LinearSolver.h"
#include "ibex_CtcFwdBwd.h"

#include <iomanip>

using namespace std;

namespace ibex {

CtcParamNewton::CtcParamNewton(Function& f, const VarSet& vars) : Ctc(f.nb_var()), f(f), vars(vars) { }

void CtcParamNewton::contract(IntervalVector& box) {
	//cout << "newton: box=" << box << " vars="<< vars << endl;
	newton(f,vars,box,NEWTON_CTC_PREC);
	//cout << "     gives=" << box << endl;
}

VarSet get_vars(Function& f, const Vector& pt, const BitSet& forced_params) {
	int n=f.nb_var();
	int m=f.image_dim();
	Matrix A=f.jacobian(pt).mid();
	Matrix LU(m,n);
	int *pr = new int[m];
	int *pc = new int[n]; // the interesting output: the variables permutation

	// To force the Gauss elimination not to choose
	// the "forced" parameters, we fill their respective
	// column with zeros
	for (int i=0; i<n; i++) {
		if (forced_params[i]) {
			A.set_col(i,Vector::zeros(m));
		}
	}

	try {
		real_LU(A,LU,pr,pc);
	} catch(SingularMatrixException& e) {
		// means in particular that we could not extract an
		// invertible m*m submatrix
		delete [] pr;
		delete [] pc;
		throw e;
	}
	// ==============================================================

	BitSet _vars=BitSet::empty(n);

	for (int i=0; i<m; i++) {
		_vars.add(pc[i]);
	}

	for (int j=0; j<n; j++) {
		assert(!(forced_params[j] && _vars[j]));
	}

	delete [] pr;
	delete [] pc;
	return VarSet(f.nb_var(),_vars);
}

IntervalVector find_solution(Function& f, IntervalVector& facet, const VarSet& vars) {
	int n=facet.size();

	double EPS=1e-08; // TODO: remove hard-coded value

	if (facet.is_empty() || facet.max_diam()<=EPS) return facet;
	//cout << "facet=" << facet << " " << facet.max_diam() << endl;

	// We explore the parameter space using a list (breadth-first search)
	// to ease diversification (our goal being to find a solution)
	//list<IntervalVector> s;
	stack<IntervalVector> s;

	IntervalVector x(vars.var_box(facet));
	IntervalVector p(vars.param_box(facet));
	IntervalVector box(n);

	s.push(p); // s.push_back(p);
	bool sol_found=false;
	int sol_maybe_lost=0;

	int iter=0;

	while (!s.empty() && !sol_found) {

		if (iter==FIND_SOLUTION_MAX_ITER) throw FindSolutionFail();

		iter++;

		p=s.top(); //front();
		s.pop(); //pop_front();

		// first try to find a solution
		box=vars.full_box(x,p.mid());

		newton(f,vars,box,NEWTON_CERTIF_PREC);

		if (!box.is_empty() && vars.var_box(box).is_interior_subset(x)) {
			sol_found=true;
		} else {
			box=vars.full_box(x,p);
			// if no solution found, try to prove there is none (contract the box)

//			CtcFwdBwd fwdbwd(f);
//			fwdbwd.contract(box);

			newton(f,vars,box,NEWTON_CTC_PREC);

			if (box.is_empty()) continue;
			else {
				if (p.max_diam()<1e-12) { // TODO: remove hard-coded value
					// It may result from the diff operations
					// that the solutions for the current values [p] of the
					// parameters lie very close to one bound of the x domain
					// so that either Newton applied on the sample value p:
					// 1- cannot contract enough to obtain a strict inclusion
					//    (solution inside)
					// 2- or, contracts it to the empty set
					//    (solution outside)
					// In both cases, we simply continue the search (until
					// the maximum number of iterations is reached). However,
					// the facet cannot be entirely contracted anymore.

					sol_maybe_lost++;
//					cerr << "[find-solution] cannot certify solution:" << endl;
//					cerr << "  vars=" << vars << endl;
//					cerr << "  p=" << p << endl;
//					cerr << "  x (before Newton)=" << x << endl;
//					cerr << "  x (after Newton) =" << vars.var_box(box) << endl;
//					cerr << "  facet=" << facet << endl;

					continue;
				}

				pair<IntervalVector,IntervalVector> _pair=p.bisect(p.extr_diam_index(false));
			//	if (iter % 2==0) {
					s.push(_pair.first);
					s.push(_pair.second);
//				} else {
//					s.push(_pair#include "ibex_LinearSolver.h".second);
//					s.push(_pair.first);
//				}
			}

		}
	}

	if (sol_found) {
		// TODO:
		// set the facet to the hull of the remaining boxes in the buffer
		return box;
	} else {
		if (!sol_maybe_lost) {
			facet.set_empty();
			return IntervalVector::empty(n);
		} else {
			// Other possible case of failure. The maximum number of iterations has
			// not been reached but no solution has been found and the search is over.
			throw FindSolutionFail();
		}
	}
}

bool is_homeomorph_half_ball(const IntervalVector& ginf, const IntervalMatrix& Dg, const IntervalVector& param_box) {

	int p=param_box.size();
	int k=Dg.nb_rows();

	Vector pinf=param_box.lb();

	bool* b=new bool[k]; // counter (to range over all orthants)
	for (int i=0; i<k; i++) b[i]=false; // start with negative signs

	Matrix Jinf=Dg.lb();
	Vector Jinf_pinf= Jinf * pinf;
	Matrix Jsup=Dg.ub();
	Vector Jsup_pinf= Jsup * pinf;

	LinearSolver linsolve(p, k);

	Interval opt(0.0); // store the optimum (unused)

	bool result=true; // has the test succeed?

	bool over=false; // true after 2^k loops

	while (!over) {

		linsolve.initBoundVar(param_box);

		for (int i=0; i<k; i++) {
			if (b[i])
				//cout << "  add constraint: " << Jinf.row(i) << "*u>=" << (Jinf_pinf[i]-ginf[i].lb()) << endl;
				linsolve.addConstraint(Jinf.row(i),GEQ,Jinf_pinf[i]-ginf[i].lb());
			else
				//cout << "  add constraint: " << Jsup.row(i) << "*u<=" << (Jsup_pinf[i]-ginf[i].ub()) << endl;
				linsolve.addConstraint(Jsup.row(i),LEQ,Jsup_pinf[i]-ginf[i].ub());
		}

		// note : "-1" just to have a strict minorant of the objective
		LinearSolver::Status_Sol stat = linsolve.run_simplex(param_box, LinearSolver::MINIMIZE, 0, opt,param_box[0].lb()-1);
		//cout << "  status=" << stat << endl;

		linsolve.cleanConst();

		if (stat != LinearSolver::OPTIMAL) {
			result=false;
			break;
		}

		// =========== increment counter ===========
		int i=k-1; // last digit
		while (i>=0 && b[i]) { // find next digit to increment
			b[i]=false; // reset digit
			i--;
		}
		if (i==-1) over=true;
		else b[i]=true; // increment digit
	}

	delete[] b;

	return result;
}

} // end namespace ibex
