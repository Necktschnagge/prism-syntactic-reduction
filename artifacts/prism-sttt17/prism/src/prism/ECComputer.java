//==============================================================================
//	
//	Copyright (c) 2002-
//	Authors:
//	* Dave Parker <david.parker@comlab.ox.ac.uk> (University of Oxford, formerly University of Birmingham)
//	* Carlos S. Bederian (Universidad Nacional de Cordoba)
//	* Mark Kattenbelt <mark.kattenbelt@comlab.ox.ac.uk> (University of Oxford)
//	
//------------------------------------------------------------------------------
//	
//	This file is part of PRISM.
//	
//	PRISM is free software; you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation; either version 2 of the License, or
//	(at your option) any later version.
//	
//	PRISM is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//	
//	You should have received a copy of the GNU General Public License
//	along with PRISM; if not, write to the Free Software Foundation,
//	Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//	
//==============================================================================

package prism;

import java.util.List;

import jdd.JDDNode;
import jdd.JDDVars;

/**
 * Abstract class for (symbolic) classes that compute (M)ECs, i.e. (maximal) end components,
 * for a nondeterministic model such as an MDP.
 */
public abstract class ECComputer extends PrismComponent
{
	// setting(s)
	protected double sumRoundOff;

	// model info
	protected JDDNode trans;
	protected JDDNode trans01;
	protected JDDNode reach;
	protected JDDVars allDDRowVars;
	protected JDDVars allDDColVars;
	protected JDDVars allDDNondetVars;

	// stuff for ECs
	protected List<JDDNode> mecs;

	/**
	 * Static method to create a new ECComputer object, depending on current settings.
	 */
	public static ECComputer createECComputer(PrismComponent parent, NondetModel model) throws PrismException
	{
		return createECComputer(parent, model.getReach(), model.getTrans(), model.getTrans01(), model.getAllDDRowVars(), model.getAllDDColVars(),
				model.getAllDDNondetVars());
	}

	/**
	 * Static method to create a new ECComputer object, depending on current settings.
	 */
	public static ECComputer createECComputer(PrismComponent parent, JDDNode reach, JDDNode trans, JDDNode trans01, JDDVars allDDRowVars, JDDVars allDDColVars,
			JDDVars allDDNondetVars) throws PrismException
	{
		// Only one algorithm implemented currently
		return new ECComputerDefault(parent, reach, trans, trans01, allDDRowVars, allDDColVars, allDDNondetVars);
	}

	/**
	 * Base constructor.
	 */
	public ECComputer(PrismComponent parent, JDDNode reach, JDDNode trans, JDDNode trans01, JDDVars allDDRowVars, JDDVars allDDColVars, JDDVars allDDNondetVars)
			throws PrismException
	{
		super(parent);
		this.reach = reach;
		this.trans = trans;
		this.trans01 = trans01;
		this.allDDRowVars = allDDRowVars;
		this.allDDColVars = allDDColVars;
		this.allDDNondetVars = allDDNondetVars;
		sumRoundOff = settings.getDouble(PrismSettings.PRISM_SUM_ROUND_OFF);

		if (jdd.SanityJDD.enabled) {
			jdd.SanityJDD.checkIsDDOverVars(trans, allDDRowVars, allDDColVars, allDDNondetVars);
			jdd.SanityJDD.checkIsDDOverVars(trans01, allDDRowVars, allDDColVars, allDDNondetVars);
			jdd.SanityJDD.checkIsDDOverVars(reach, allDDRowVars);
		}
	}

	/**
	 * Compute states of all maximal end components (MECs) and store them.
	 * They can be retrieved using {@link #getMECStates()}.
	 * You will need to to deref these afterwards.
	 */
	public abstract void computeMECStates() throws PrismException;

	/**
	 * Compute states of all maximal end components (MECs) in the submodel obtained
	 * by restricting this one to the set of states {@code restrict}, and store them.
	 * They can be retrieved using {@link #getMECStates()}.
	 * You will need to to deref these afterwards.
	 * If {@code restrict} is null, we look at the whole model, not a submodel.
	 * @param restrict BDD for the set of states to restrict to
	 */
	public abstract void computeMECStates(JDDNode restrict) throws PrismException;

	/**
	 * Compute states of all accepting maximal end components (MECs) in the submodel obtained
	 * by restricting this one to the set of states {@code restrict}, and store them,
	 * where acceptance is defined as those which intersect with {@code accept}.
	 * They can be retrieved using {@link #getMECStates()}.
	 * You will need to to deref these afterwards.
	 * If {@code restrict} is null, we look at the whole model, not a submodel.
	 * If {@code accept} is null, the acceptance condition is trivially satisfied.
	 * @param restrict BDD for the set of states to restrict to
	 * @param accept BDD for the set of accepting states
	 */
	public abstract void computeMECStates(JDDNode restrict, JDDNode accept) throws PrismException;

	/**
	 * Get the list of states for computed MECs.
	 * You need to deref these BDDs when you are finished with them.
	 */
	public List<JDDNode> getMECStates()
	{
		return mecs;
	}

	/**
	 * Returns the maximal stable set of states contained in {@code candidateStates},
	 * i.e. the maximal subset of states which have a choice whose transitions remain in the subset.
	 *
	 * <br>[ REFS: <i>result</i>, DEREFS: candidateStates ]
	 * @param candidateStates BDD for a set of states (over allDDRowVars)
	 * @return A referenced BDD with the maximal stable set in candidateStates
	 */
	public abstract JDDNode findMaximalStableSet(JDDNode candidateStates);

	/**
	 * Get the set of transitions (s,alpha,t) that remain in the state set,
	 * i.e., with a choice alpha that ensures that the successor states
	 * are all in the state set again.
	 *
	 * Returns a 0/1-MTBDD over row, col and nondet vars.
	 *
	 * <br>[ REFS: <i>result</i>, DEREFS: <i>stateSet</i> ]
	 * @param stateSet state set DD over the row variables
	 */
	public abstract JDDNode getStableTransitions(JDDNode stateSet);
}
