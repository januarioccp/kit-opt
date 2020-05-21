// -------------------------------------------------------------- -*- C++ -*-
// File: examples/test/ilocplex/tsp.cpp
// Version 12.5
// --------------------------------------------------------------------------
// Licensed Materials - Property of IBM
// 5725-A06 5725-A29 5724-Y48 5724-Y49 5724-Y54 5724-Y55 5655-Y21
// Copyright IBM Corporation 2000, 2012. All Rights Reserved.
//
// US Government Users Restricted Rights - Use, duplication or
// disclosure restricted by GSA ADP Schedule Contract with
// IBM Corp.
// --------------------------------------------------------------------------
//
//
// tsp.cpp -- Solves a simple traveling salesman problem.
//
// This example solves a small traveling salesman problem 
// as a MIP model containing two sets of constraints:
//   --- degree:  each city is visited once
//                (exactly two edges are incident to each
//                 node in the graph representation)
//   --- subtour: a tour contains no subtours
//                (given a proper subset of the nodes of size
//                 L, the set of edges between pairs of nodes
//                 in this set is of size no more than L-1)
// All of the degree constraints are included in the initial
// MIP, while subtour constraints are treated as lazy constraints,
// and are added dynamically through the cut callback.

#include <ilcplex/ilocplex.h>
#include "data.h"
#include <vector>
using namespace std;

#ifdef FULLTEST
#include <assert.h>
IloBool cutCalled = IloFalse;
#endif

ILOSTLBEGIN

typedef IloArray<IloIntVarArray> Edges;

IloInt checkTour(IloNumArray2 sol, IloNumArray seen, IloNum tol) {
   IloInt j, n    = sol.getSize();
   IloInt last    = -1;
   IloInt length  = 0;
   IloInt current = 0;
   seen.clear();
   seen.add(n, 0.0);

   // Search for a subtour if sol[] is integer feasible

   while ( seen[current] == 0 ) {
      length++;
      seen[current] = length;
      for (j = 0; j < current; j++) {
         if ( j != last  &&  sol[current][j] >= 1.0-tol ) break;
      }
      if ( j == current ) {
         for (j = current+1; j < n; j++) {
            if ( j != last  &&  sol[j][current] >= 1.0-tol ) break;
         }
      }
      if ( j == n ) return (n+1);
      last    = current;
      current = j;
   }

   return (length);
}


ILOLAZYCONSTRAINTCALLBACK2(SubtourEliminationCallback, Edges, x, IloNum, tol) {
   IloInt i, j;
   IloInt n   = x.getSize();
   IloEnv env = getEnv();
   IloNumArray2 sol(env, n);
   for (i = 0; i < n; i++) {
      sol[i] = IloNumArray(env);
      getValues(sol[i], x[i]);
   }

   IloNumArray seen(env);
   IloInt length = checkTour(sol, seen, tol);
   if ( length >= n ) {
      seen.end();
      for (i = 0; i < n; i++)
         sol[i].end();
      sol.end();
      return;
   }

   // Create and add subtour constraint ---
   // No more than 'length-1' edges between members of the subtour

   IloExpr clique(env);
   for (i = 0; i < n; i++) {
      if ( seen[i] ) {
         for (j = i+1; j < n; j++) {
            if ( seen[j] )
               clique += x[j][i];
         }
      }
   }
   add(clique <= length-1).end();
   clique.end();
   seen.end();
#ifdef FULLTEST
   cutCalled = IloTrue;
#endif

   for (i = 0; i < n; i++)
      sol[i].end();
   sol.end();

   return;
}


int main(int argc, char **argv) {
   Data input(argc, argv[1]);
   input.readData();
   
   IloInt n = input.getDimension();

   IloEnv env;
   try {   
      IloNumArray2 dist(env,n);
      
      IloInt i, j;
      for(i=0; i < n; i++)
         dist[i] = IloNumArray(env, n);

      for(i=0; i < n; i++)
         for(j=0; j < n; j++)
            dist[i][j] = input.getDistance(i,j);

      // create model

      IloModel tsp(env);
#ifdef FULLTEST
      IloCplex cpxtest(tsp);
#endif

      // Create variables x[i][j] for all 0 <= j < i < n representing the
      // edge between cities i and j.  A setting of 1 indicates that the edge
      // is part of the tour.

      Edges x(env, n);
      for (i = 0; i < n; i++)
         x[i] = IloIntVarArray(env, n, 0, 1);

      // Objective is to minimize the sum of edge weights
      // for traveled edges

      IloExpr obj(env);
      for (i = 1; i < n; i++){
         obj += IloScalProd(x[i], dist[i]);
      }
      tsp.add(IloMinimize(env, obj));

      // degree constraints --- exactly two traveled edges
      // incident to each node

      for (i = 0; i < n; i++) {
         IloExpr expr(env);
         for (j = 0;   j < i; j++)  expr += x[i][j];
         for (j = i+1; j < n; j++)  expr += x[j][i];
         tsp.add(expr == 2);
      }

      IloCplex cplex(tsp);

      IloNum tol = cplex.getParam(IloCplex::EpInt);
      IloCplex::Callback sec = cplex.use(
                                  SubtourEliminationCallback(env, x, tol));

      cplex.setParam(IloCplex::PreInd, IloFalse);
      if ( cplex.solve() )
         env.out() << "Optimal tour length "
                   << cplex.getObjValue() << endl;

      IloNumArray2 sol(env, n);
      for (i = 0; i < n; i++) {
         sol[i] = IloNumArray(env);
         cplex.getValues(sol[i], x[i]);
      }
      IloNumArray seen(env);
      IloInt length = checkTour(sol, seen, tol);

      if ( length < n ) {
         IloExpr clique(env);
         for (i = 0; i < n; i++) {
            if ( seen[i] ) {
               for (j = i+1; j < n; j++) {
                  if ( seen[j] ) clique += x[j][i];
               }
            }
         }
         cerr << cplex.getValue(clique) << " <= " << length-1 << endl;
      }

      assert (length == n);

#ifdef FULLTEST
      assert ( cplex.getImpl()->isConsistent() );
      assert ( cpxtest.getImpl()->isConsistent() );
      assert ( cplex.getStatus() == IloAlgorithm::Optimal );
      assert ( fabs (cplex.getObjValue() - 11461.0) < 1e-6 );
      assert (cutCalled);
      env.out() << "Test completed successfully" << endl;
#endif

      sec.end();
   }
   catch (const IloException& e) {
      cerr << "Exception caught: " << e << endl;
#ifdef FULLTEST
      assert (0);
#endif
   }
   catch (...) {
      cerr << "Unknown exception caught!" << endl;
#ifdef FULLTEST
      assert (0);
#endif
   }

   env.end();
   return 0;
}

