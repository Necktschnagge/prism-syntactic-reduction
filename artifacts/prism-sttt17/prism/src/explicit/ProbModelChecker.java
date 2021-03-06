//==============================================================================
//	
//	Copyright (c) 2002-
//	Authors:
//	* Dave Parker <david.parker@comlab.ox.ac.uk> (University of Oxford)
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

package explicit;

import java.io.File;
import java.util.ArrayList;
import java.util.BitSet;
import java.util.List;
import java.util.concurrent.ForkJoinPool;

import common.iterable.collections.SetFactory.SupportedImplementations;

import parser.Values;
import parser.ast.Coalition;
import parser.ast.Expression;
import parser.ast.ExpressionProb;
import parser.ast.ExpressionQuantileProb;
import parser.ast.ExpressionQuantileProbNormalForm;
import parser.ast.ExpressionReward;
import parser.ast.ExpressionSS;
import parser.ast.ExpressionStrategy;
import parser.ast.ExpressionTemporal;
import parser.ast.ExpressionUnaryOp;
import parser.ast.ModulesFile;
import parser.ast.RewardStruct;
import parser.ast.TemporalOperatorBound;
import parser.ast.TemporalOperatorBounds;
import parser.type.TypeBool;
import parser.type.TypeDouble;
import parser.type.TypePathBool;
import parser.type.TypePathDouble;
import prism.IntegerBound;
import prism.ModelType;
import prism.OpRelOpBound;
import prism.PrismComponent;
import prism.PrismException;
import prism.PrismLangException;
import prism.PrismNotSupportedException;
import prism.PrismSettings;
import explicit.quantile.QuantileTransformations;
import explicit.quantile.QuantileUtilities;
import explicit.quantile.context.helpers.multiStateSolutions.MultiStateSolutionMethod;
import explicit.quantile.topologicalSorting.TopologicalSorting.QuantileSccMethod;
import explicit.rewards.ConstructRewards;
import explicit.rewards.MCRewards;
import explicit.rewards.MDPRewards;
import explicit.rewards.Rewards;
import explicit.rewards.STPGRewards;

/**
 * Super class for explicit-state probabilistic model checkers.
 */
public class ProbModelChecker extends NonProbModelChecker
{
	// Flags/settings
	// (NB: defaults do not necessarily coincide with PRISM)

	// Method used to solve linear equation systems
	protected LinEqMethod linEqMethod = LinEqMethod.GAUSS_SEIDEL;
	// Method used to solve MDPs
	protected MDPSolnMethod mdpSolnMethod = MDPSolnMethod.GAUSS_SEIDEL;
	// Iterative numerical method termination criteria
	protected TermCrit termCrit = TermCrit.RELATIVE;
	// Parameter for iterative numerical method termination criteria
	protected double termCritParam = 1e-8;
	// Max iterations for numerical solution
	protected int maxIters = 100000;
	// Use precomputation algorithms in model checking?
	protected boolean precomp = true;
	protected boolean prob0 = true;
	protected boolean prob1 = true;
	// Use predecessor relation? (e.g. for precomputation)
	protected boolean preRel = true;
	// Direction of convergence for value iteration (lfp/gfp)
	protected ValIterDir valIterDir = ValIterDir.BELOW;
	// Method used for numerical solution
	protected SolnMethod solnMethod = SolnMethod.VALUE_ITERATION;
	// Is non-convergence of an iterative method an error?
	protected boolean errorOnNonConverge = true;
	// Adversary export
	protected boolean exportAdv = false;
	protected String exportAdvFilename;

	// Enums for flags/settings

	// Method used for numerical solution
	public enum LinEqMethod {
		POWER, JACOBI, GAUSS_SEIDEL, BACKWARDS_GAUSS_SEIDEL, JOR, SOR, BACKWARDS_SOR;
		public String fullName()
		{
			switch (this) {
			case POWER:
				return "Power method";
			case JACOBI:
				return "Jacobi";
			case GAUSS_SEIDEL:
				return "Gauss-Seidel";
			case BACKWARDS_GAUSS_SEIDEL:
				return "Backwards Gauss-Seidel";
			case JOR:
				return "JOR";
			case SOR:
				return "SOR";
			case BACKWARDS_SOR:
				return "Backwards SOR";
			default:
				return this.toString();
			}
		}
	};

	// Method used for solving MDPs
	public enum MDPSolnMethod {
		VALUE_ITERATION, GAUSS_SEIDEL, POLICY_ITERATION, MODIFIED_POLICY_ITERATION, LINEAR_PROGRAMMING;
		public String fullName()
		{
			switch (this) {
			case VALUE_ITERATION:
				return "Value iteration";
			case GAUSS_SEIDEL:
				return "Gauss-Seidel";
			case POLICY_ITERATION:
				return "Policy iteration";
			case MODIFIED_POLICY_ITERATION:
				return "Modified policy iteration";
			case LINEAR_PROGRAMMING:
				return "Linear programming";
			default:
				return this.toString();
			}
		}
	};

	// Iterative numerical method termination criteria
	public enum TermCrit {
		ABSOLUTE, RELATIVE
	};

	// Direction of convergence for value iteration (lfp/gfp)
	public enum ValIterDir {
		BELOW, ABOVE
	};

	// Method used for numerical solution
	public enum SolnMethod {
		VALUE_ITERATION, GAUSS_SEIDEL, POLICY_ITERATION, MODIFIED_POLICY_ITERATION, LINEAR_PROGRAMMING
	};

	/**
	 * Create a new ProbModelChecker, inherit basic state from parent (unless null).
	 */
	public ProbModelChecker(PrismComponent parent) throws PrismException
	{
		super(parent);

		// If present, initialise settings from PrismSettings
		if (settings != null) {
			String s;
			// PRISM_LIN_EQ_METHOD
			s = settings.getString(PrismSettings.PRISM_LIN_EQ_METHOD);
			if (s.equals("Power")) {
				setLinEqMethod(LinEqMethod.POWER);
			} else if (s.equals("Jacobi")) {
				setLinEqMethod(LinEqMethod.JACOBI);
			} else if (s.equals("Gauss-Seidel")) {
				setLinEqMethod(LinEqMethod.GAUSS_SEIDEL);
			} else if (s.equals("Backwards Gauss-Seidel")) {
				setLinEqMethod(LinEqMethod.BACKWARDS_GAUSS_SEIDEL);
			} else if (s.equals("JOR")) {
				setLinEqMethod(LinEqMethod.JOR);
			} else if (s.equals("SOR")) {
				setLinEqMethod(LinEqMethod.SOR);
			} else if (s.equals("Backwards SOR")) {
				setLinEqMethod(LinEqMethod.BACKWARDS_SOR);
			} else {
				throw new PrismNotSupportedException("Explicit engine does not support linear equation solution method \"" + s + "\"");
			}
			// PRISM_MDP_SOLN_METHOD
			s = settings.getString(PrismSettings.PRISM_MDP_SOLN_METHOD);
			if (s.equals("Value iteration")) {
				setMDPSolnMethod(MDPSolnMethod.VALUE_ITERATION);
			} else if (s.equals("Gauss-Seidel")) {
				setMDPSolnMethod(MDPSolnMethod.GAUSS_SEIDEL);
			} else if (s.equals("Policy iteration")) {
				setMDPSolnMethod(MDPSolnMethod.POLICY_ITERATION);
			} else if (s.equals("Modified policy iteration")) {
				setMDPSolnMethod(MDPSolnMethod.MODIFIED_POLICY_ITERATION);
			} else if (s.equals("Linear programming")) {
				setMDPSolnMethod(MDPSolnMethod.LINEAR_PROGRAMMING);
			} else {
				throw new PrismNotSupportedException("Explicit engine does not support MDP solution method \"" + s + "\"");
			}
			// PRISM_TERM_CRIT
			s = settings.getString(PrismSettings.PRISM_TERM_CRIT);
			if (s.equals("Absolute")) {
				setTermCrit(TermCrit.ABSOLUTE);
			} else if (s.equals("Relative")) {
				setTermCrit(TermCrit.RELATIVE);
			} else {
				throw new PrismNotSupportedException("Unknown termination criterion \"" + s + "\"");
			}
			// PRISM_TERM_CRIT_PARAM
			setTermCritParam(settings.getDouble(PrismSettings.PRISM_TERM_CRIT_PARAM));
			// PRISM_MAX_ITERS
			setMaxIters(settings.getInteger(PrismSettings.PRISM_MAX_ITERS));
			// PRISM_PRECOMPUTATION
			setPrecomp(settings.getBoolean(PrismSettings.PRISM_PRECOMPUTATION));
			// PRISM_PROB0
			setProb0(settings.getBoolean(PrismSettings.PRISM_PROB0));
			// PRISM_PROB1
			setProb1(settings.getBoolean(PrismSettings.PRISM_PROB1));
			// PRISM_USE_PRE
			setPreRel(settings.getBoolean(PrismSettings.PRISM_PRE_REL));

			// PRISM_EXPORT_ADV
			s = settings.getString(PrismSettings.PRISM_EXPORT_ADV);
			if (!(s.equals("None")))
				setExportAdv(true);
			// PRISM_EXPORT_ADV_FILENAME
			setExportAdvFilename(settings.getString(PrismSettings.PRISM_EXPORT_ADV_FILENAME));

			// Quantile Options
			setQuantileSccMethod(settings.getString(PrismSettings.QUANTILE_SCC_METHOD));
			setSetFactory(settings.getString(PrismSettings.QUANTILE_SET_FACTORY));
			setAdaptiveSetThreshold(settings.getInteger(PrismSettings.QUANTILE_ADAPTIVE_SET_THRESHOLD));
			setMultiStateSolution(settings.getString(PrismSettings.QUANTILE_MULTI_STATE_METHOD));
			setUseZeroRewardTryAndSet(settings.getBoolean(PrismSettings.QUANTILE_USE_ZERO_REWARD_TRY_AND_SET));
			setUseLpSolverOnly(settings.getBoolean(PrismSettings.QUANTILE_USE_LP_SOLVER_ONLY));
			setLpSolverBound(settings.getInteger(PrismSettings.QUANTILE_LP_SOLVER_ONLY_BOUND));
			setCalculatePositiveRewardStatesInParallel(settings.getBoolean(PrismSettings.QUANTILE_POSITIVE_REWARDS_PARALLEL));
			setCalculateZeroRewardStatesInParallel(settings.getBoolean(PrismSettings.QUANTILE_ZERO_REWARDS_PARALLEL));
			setWorkerPool(settings.getInteger(PrismSettings.QUANTILE_PARALLEL_POOL_SIZE));
			setDebugLevel(settings.getInteger(PrismSettings.QUANTILE_DEBUG_LEVEL));
			setQuantileVerifyResult(settings.getBoolean(PrismSettings.QUANTILE_VERIFY_RESULT));
			setQuantileUseQuantitative(settings.getBoolean(PrismSettings.QUANTILE_USE_QUANTITATIVE));
		}
	}

	// Settings methods

	/**
	 * Inherit settings (and the log) from another ProbModelChecker object.
	 * For model checker objects that inherit a PrismSettings object, this is superfluous
	 * since this has been done already.
	 */
	public void inheritSettings(ProbModelChecker other)
	{
		super.inheritSettings(other);
		setLinEqMethod(other.getLinEqMethod());
		setMDPSolnMethod(other.getMDPSolnMethod());
		setTermCrit(other.getTermCrit());
		setTermCritParam(other.getTermCritParam());
		setMaxIters(other.getMaxIters());
		setPrecomp(other.getPrecomp());
		setProb0(other.getProb0());
		setProb1(other.getProb1());
		setValIterDir(other.getValIterDir());
		setSolnMethod(other.getSolnMethod());
		setErrorOnNonConverge(other.geterrorOnNonConverge());
	}

	/**
	 * Print summary of current settings.
	 */
	public void printSettings()
	{
		super.printSettings();
		mainLog.print("linEqMethod = " + linEqMethod + " ");
		mainLog.print("mdpSolnMethod = " + mdpSolnMethod + " ");
		mainLog.print("termCrit = " + termCrit + " ");
		mainLog.print("termCritParam = " + termCritParam + " ");
		mainLog.print("maxIters = " + maxIters + " ");
		mainLog.print("precomp = " + precomp + " ");
		mainLog.print("prob0 = " + prob0 + " ");
		mainLog.print("prob1 = " + prob1 + " ");
		mainLog.print("valIterDir = " + valIterDir + " ");
		mainLog.print("solnMethod = " + solnMethod + " ");
		mainLog.print("errorOnNonConverge = " + errorOnNonConverge + " ");
	}

	// Set methods for flags/settings

	/**
	 * Set verbosity level, i.e. amount of output produced.
	 */
	public void setVerbosity(int verbosity)
	{
		this.verbosity = verbosity;
	}

	/**
	 * Set method used to solve linear equation systems.
	 */
	public void setLinEqMethod(LinEqMethod linEqMethod)
	{
		this.linEqMethod = linEqMethod;
	}

	/**
	 * Set method used to solve MDPs.
	 */
	public void setMDPSolnMethod(MDPSolnMethod mdpSolnMethod)
	{
		this.mdpSolnMethod = mdpSolnMethod;
	}

	/**
	 * Set termination criteria type for numerical iterative methods.
	 */
	public void setTermCrit(TermCrit termCrit)
	{
		this.termCrit = termCrit;
	}

	/**
	 * Set termination criteria parameter (epsilon) for numerical iterative methods.
	 */
	public void setTermCritParam(double termCritParam)
	{
		this.termCritParam = termCritParam;
	}

	/**
	 * Set maximum number of iterations for numerical iterative methods.
	 */
	public void setMaxIters(int maxIters)
	{
		this.maxIters = maxIters;
	}

	/**
	 * Set whether or not to use precomputation (Prob0, Prob1, etc.).
	 */
	public void setPrecomp(boolean precomp)
	{
		this.precomp = precomp;
	}

	/**
	 * Set whether or not to use Prob0 precomputation
	 */
	public void setProb0(boolean prob0)
	{
		this.prob0 = prob0;
	}

	/**
	 * Set whether or not to use Prob1 precomputation
	 */
	public void setProb1(boolean prob1)
	{
		this.prob1 = prob1;
	}

	/**
	 * Set whether or not to use pre-computed predecessor relation
	 */
	public void setPreRel(boolean preRel)
	{
		this.preRel = preRel;
	}

	/**
	 * Set direction of convergence for value iteration (lfp/gfp).
	 */
	public void setValIterDir(ValIterDir valIterDir)
	{
		this.valIterDir = valIterDir;
	}

	/**
	 * Set method used for numerical solution.
	 */
	public void setSolnMethod(SolnMethod solnMethod)
	{
		this.solnMethod = solnMethod;
	}

	/**
	 * Set whether non-convergence of an iterative method an error
	 */
	public void setErrorOnNonConverge(boolean errorOnNonConverge)
	{
		this.errorOnNonConverge = errorOnNonConverge;
	}

	public void setExportAdv(boolean exportAdv)
	{
		this.exportAdv = exportAdv;
	}

	public void setExportAdvFilename(String exportAdvFilename)
	{
		this.exportAdvFilename = exportAdvFilename;
	}

	// Get methods for flags/settings

	public int getVerbosity()
	{
		return verbosity;
	}

	public LinEqMethod getLinEqMethod()
	{
		return linEqMethod;
	}

	public MDPSolnMethod getMDPSolnMethod()
	{
		return mdpSolnMethod;
	}

	public TermCrit getTermCrit()
	{
		return termCrit;
	}

	public double getTermCritParam()
	{
		return termCritParam;
	}

	public int getMaxIters()
	{
		return maxIters;
	}

	public boolean getPrecomp()
	{
		return precomp;
	}

	public boolean getProb0()
	{
		return prob0;
	}

	public boolean getProb1()
	{
		return prob1;
	}

	public boolean getPreRel()
	{
		return preRel;
	}

	public ValIterDir getValIterDir()
	{
		return valIterDir;
	}

	public SolnMethod getSolnMethod()
	{
		return solnMethod;
	}

	/**
	 * Is non-convergence of an iterative method an error?
	 */
	public boolean geterrorOnNonConverge()
	{
		return errorOnNonConverge;
	}

	// Model checking functions

	@Override
	public StateValues checkExpression(Model model, Expression expr, BitSet statesOfInterest) throws PrismException
	{
		StateValues res;

		// <<>> or [[]] operator
		if (expr instanceof ExpressionStrategy) {
			res = checkExpressionStrategy(model, (ExpressionStrategy) expr, statesOfInterest);
		}
		// P operator
		else if (expr instanceof ExpressionProb) {
			res = checkExpressionProb(model, (ExpressionProb) expr, statesOfInterest);
		}
		// R operator
		else if (expr instanceof ExpressionReward) {
			res = checkExpressionReward(model, (ExpressionReward) expr, statesOfInterest);
		}
		// S operator
		else if (expr instanceof ExpressionSS) {
			res = checkExpressionSteadyState(model, (ExpressionSS) expr);
		}
		// Quantile Operator
		else if (expr instanceof ExpressionQuantileProbNormalForm) {
			res = checkExpressionQuantile(model, (ExpressionQuantileProbNormalForm) expr, statesOfInterest);
		}
		// Quantile Operator
		else if (expr instanceof ExpressionQuantileProb) {
			res = checkExpressionQuantile(model, (ExpressionQuantileProb) expr, statesOfInterest);
		}
		// Otherwise, use the superclass
		else {
			res = super.checkExpression(model, expr, statesOfInterest);
		}

		return res;
	}

	// Quantile calculations
	protected QuantileSccMethod quantileSccMethod = QuantileSccMethod.TARJAN_ITERATIVE;
	protected SupportedImplementations setFactory = SupportedImplementations.ADAPTIVE_SINGLETON_SET;
	protected int adaptiveSetThreshold = 1;
	protected MultiStateSolutionMethod multiStateSolutionMethod = MultiStateSolutionMethod.VALUE_ITERATION;
	protected boolean useZeroRewardTryAndSet = true;
	protected int lpSolverBound = 10;
	protected boolean useLpSolverOnly = false;
	protected boolean calculatePositiveRewardStatesInParallel = false;
	protected boolean calculateZeroRewardStatesInParallel = false;
	protected int debugLevel = 0;
	protected boolean quantileVerifyResult = false;
	protected boolean quantileUseQuantitative = false;
	protected ForkJoinPool workerPool = null;

	public void setMultiStateSolution(String theMultiStateSolutionMethod)
	{
		multiStateSolutionMethod = MultiStateSolutionMethod.valueOf(theMultiStateSolutionMethod);
	}

	public MultiStateSolutionMethod getMultiStateSolutionMethod()
	{
		return multiStateSolutionMethod;
	}

	public void setUseZeroRewardTryAndSet(boolean doUseZeroRewardTryAndSet)
	{
		useZeroRewardTryAndSet = doUseZeroRewardTryAndSet;
	}

	public boolean useZeroRewardTryAndSet()
	{
		return useZeroRewardTryAndSet;
	}

	public void setUseLpSolverOnly(boolean doUseLpSolverOnly)
	{
		useLpSolverOnly = doUseLpSolverOnly;
	}

	public boolean usesLpSolverOnly()
	{
		return useLpSolverOnly;
	}

	public void setLpSolverBound(int bound)
	{
		lpSolverBound = bound;
	}

	public int getLpSolverBound()
	{
		return lpSolverBound;
	}

	public void setWorkerPool(final int parallelism)
	{
		if (calculatePositiveRewardStatesInParallel || calculateZeroRewardStatesInParallel){
			workerPool = new ForkJoinPool(parallelism);
		}
	}

	public ForkJoinPool getWorkerPool()
	{
		return workerPool;
	}

	public void setCalculatePositiveRewardStatesInParallel(boolean value)
	{
		calculatePositiveRewardStatesInParallel = value;
	}

	public boolean calculatePositiveRewardStatesInParallel()
	{
		return calculatePositiveRewardStatesInParallel;
	}

	public void setCalculateZeroRewardStatesInParallel(boolean value)
	{
		calculateZeroRewardStatesInParallel = value;
	}

	public boolean calculateZeroRewardStatesInParallel()
	{
		return calculateZeroRewardStatesInParallel;
	}

	public void setDebugLevel(int debug)
	{
		debugLevel = debug;
	}

	public int getDebugLevel()
	{
		return debugLevel;
	}

	public void setQuantileVerifyResult(boolean verifyResult)
	{
		quantileVerifyResult = verifyResult;
	}

	public boolean getQuantileVerifyResult()
	{
		return quantileVerifyResult;
	}

	public void setQuantileUseQuantitative(boolean useQuantitative)
	{
		quantileUseQuantitative = useQuantitative;
	}

	public boolean getQuantileUseQuantitative()
	{
		return quantileUseQuantitative;
	}

	public void setQuantileSccMethod(String sccMethod)
	{
		quantileSccMethod = QuantileSccMethod.valueOf(sccMethod);
	}

	public QuantileSccMethod getQuantileSccMethod()
	{
		return quantileSccMethod;
	}

	public void setAdaptiveSetThreshold(int threshold) throws PrismException
	{
		if (threshold < 1)
			throw new PrismException("The threshold for adaptive sets must be greater 0");
		adaptiveSetThreshold = threshold;
	}

	public int getAdaptiveSetThreshold()
	{
		return adaptiveSetThreshold;
	}

	public void setSetFactory(String factory)
	{
		setFactory = SupportedImplementations.valueOf(factory);
	}

	public SupportedImplementations getSetFactory()
	{
		return setFactory;
	}

	public Values getConstantValues()
	{
		return constantValues;
	}

	public ModulesFile getModulesFile()
	{
		return modulesFile;
	}

	protected StateValues checkExpressionQuantile(final Model model, final ExpressionQuantileProb expressionQuantile, final BitSet statesOfInterest)
			throws PrismException
	{
		ModelExpressionTransformation<Model, Model> transformed = QuantileTransformations.toNormalForm(this, model, expressionQuantile, statesOfInterest);

		mainLog.println("Normal Form: " + transformed.getTransformedExpression() + "\n");
		final QuantileUtilities quantileUtilities = new QuantileUtilities(this);
		StateValues result;
		if (settings.getBoolean(PrismSettings.QUANTILE_NAIVE_COMPUTATION) || model.getModelType() == ModelType.CTMC){
			result = quantileUtilities.checkExpressionQuantileNaive(transformed.getTransformedModel(),
					                                                (ExpressionQuantileProbNormalForm) transformed.getTransformedExpression(),
					                                                transformed.getTransformedStatesOfInterest()
					                                               );
		} else {
			result = quantileUtilities.checkExpressionQuantile(transformed.getTransformedModel(),
					                                           (ExpressionQuantileProbNormalForm) transformed.getTransformedExpression(),
					                                           transformed.getTransformedStatesOfInterest()
					                                          );
		}
		result = transformed.projectToOriginalModel(result);

		if (getQuantileVerifyResult()) {
			QuantileUtilities.verifyResult(this, model, expressionQuantile, result, statesOfInterest);
		}

		return result;
	}

	protected StateValues checkExpressionQuantile(final Model model, final ExpressionQuantileProbNormalForm expressionQuantile, final BitSet statesOfInterest)
			throws PrismException
	{
		final QuantileUtilities quantileUtilities = new QuantileUtilities(this);
		final StateValues result;
		if (settings.getBoolean(PrismSettings.QUANTILE_NAIVE_COMPUTATION) || model.getModelType() == ModelType.CTMC){
			result = quantileUtilities.checkExpressionQuantileNaive(model, expressionQuantile, statesOfInterest);
		} else {
			result = quantileUtilities.checkExpressionQuantile(model, expressionQuantile, statesOfInterest);
		}

		if (getQuantileVerifyResult()) {
			quantileUtilities.verifyResult(this, model, expressionQuantile, result, statesOfInterest);
		}

		return result;
	}

	/**
	 * Model check a <<>> or [[]] operator expression and return the values for the statesOfInterest.
	 * * @param statesOfInterest the states of interest, see checkExpression()
	 */
	protected StateValues checkExpressionStrategy(Model model, ExpressionStrategy expr, BitSet statesOfInterest) throws PrismException
	{
		// Only support <<>>/[[]] for MDPs right now
		if (!(this instanceof MDPModelChecker))
			throw new PrismNotSupportedException("The " + expr.getOperatorString() + " operator is only supported for MDPs currently");

		// Will we be quantifying universally or existentially over strategies/adversaries?
		boolean forAll = !expr.isThereExists();
		
		// Extract coalition info
		Coalition coalition = expr.getCoalition();
		// For non-games (i.e., models with a single player), deal with the coalition operator here and then remove it
		if (coalition != null && !model.getModelType().multiplePlayers()) {
			if (coalition.isEmpty()) {
				// An empty coalition negates the quantification ("*" has no effect)
				forAll = !forAll;
			}
			coalition = null;
		}

		// For now, just support a single expression (which may encode a Boolean combination of objectives)
		List<Expression> exprs = expr.getOperands();
		if (exprs.size() > 1) {
			throw new PrismException("Cannot currently check strategy operators wth lists of expressions");
		}
		Expression exprSub = exprs.get(0);
		// Pass onto relevant method:
		// P operator
		if (exprSub instanceof ExpressionProb) {
			return checkExpressionProb(model, (ExpressionProb) exprSub, forAll, coalition, statesOfInterest);
		}
		// R operator
		else if (exprSub instanceof ExpressionReward) {
			return checkExpressionReward(model, (ExpressionReward) exprSub, forAll, coalition, statesOfInterest);
		}
		// Anything else is an error 
		else {
			throw new PrismException("Unexpected operators in " + expr.getOperatorString() + " operator");
		}
	}

	/**
	 * Model check a P operator expression and return the values for the statesOfInterest.
 	 * @param statesOfInterest the states of interest, see checkExpression()
	 */
	protected StateValues checkExpressionProb(Model model, ExpressionProb expr, BitSet statesOfInterest) throws PrismException
	{
		// Use the default semantics for a standalone P operator
		// (i.e. quantification over all strategies, and no game-coalition info)
		return checkExpressionProb(model, expr, true, null, statesOfInterest);
	}
	
	/**
	 * Model check a P operator expression and return the values for the states of interest.
	 * @param model The model
	 * @param expr The P operator expression
	 * @param forAll Are we checking "for all strategies" (true) or "there exists a strategy" (false)? [irrelevant for numerical (=?) queries] 
	 * @param coalition If relevant, info about which set of players this P operator refers to (null if irrelevant)
	 * @param statesOfInterest the states of interest, see checkExpression()
	 */
	protected StateValues checkExpressionProb(Model model, ExpressionProb expr, boolean forAll, Coalition coalition, BitSet statesOfInterest) throws PrismException
	{
		// Get info from P operator
		OpRelOpBound opInfo = expr.getRelopBoundInfo(constantValues);
		if (opInfo.hasMultipleBounds()) {
			throw new PrismNotSupportedException("Can not currently check multi-threshold expression: "+expr);
		}
		MinMax minMax = opInfo.getMinMax(model.getModelType(), forAll);

		// Compute probabilities
		StateValues probs = checkProbPathFormula(model, expr.getExpression(), minMax, statesOfInterest);

		// Print out probabilities
		if (getVerbosity() > 5) {
			mainLog.print("\nProbabilities (non-zero only) for all states:\n");
			probs.print(mainLog);
		}

		// For =? properties, just return values
		if (opInfo.isNumeric()) {
			return probs;
		}
		// Otherwise, compare against bound to get set of satisfying states
		else {
			BitSet sol = probs.getBitSetFromInterval(opInfo.getRelOp(), opInfo.getBound());
			probs.clear();
			return StateValues.createFromBitSet(sol, model);
		}
	}

	/**
	 * Compute probabilities for the contents of a P operator.
	 * @param statesOfInterest the states of interest, see checkExpression()
	 */
	protected StateValues checkProbPathFormula(Model model, Expression expr, MinMax minMax, BitSet statesOfInterest) throws PrismException
	{
		// Test whether this is a simple path formula (i.e. PCTL)
		// and whether we want to use the corresponding algorithms
		boolean useSimplePathAlgo = expr.isSimplePathFormula();

		if (useSimplePathAlgo &&
		    settings.getBoolean(PrismSettings.PRISM_PATH_VIA_AUTOMATA) &&
		    LTLModelChecker.isSupportedLTLFormula(model.getModelType(), expr)) {
			// If PRISM_PATH_VIA_AUTOMATA is true, we want to use the LTL engine
			// whenever possible
			useSimplePathAlgo = false;
		}

		if (useSimplePathAlgo) {
			return checkProbPathFormulaSimple(model, expr, minMax, statesOfInterest);
		} else {
			return checkProbPathFormulaLTL(model, expr, false, minMax, statesOfInterest);
		}
	}

	/**
	 * Compute probabilities for a simple, non-LTL path operator.
	 */
	protected StateValues checkProbPathFormulaSimple(Model model, Expression expr, MinMax minMax, BitSet statesOfInterest) throws PrismException
	{
		boolean negated = false;
		StateValues probs = null;

		expr = Expression.convertSimplePathFormulaToCanonicalForm(expr);
		ExpressionTemporal exprTemp = Expression.getTemporalOperatorForSimplePathFormula(expr);
		
		// Here, in the generic method, reward bounds and multiple bounds are not handled.
		// If they can be handled, they will have been processed already by a checkProbPathFormulaSimple
		// function in a derived model checker class.
		if (exprTemp.getBounds().hasRewardBounds())
			throw new PrismNotSupportedException("Reward bounds are not supported in this case for " + model.getModelType());
		if (model.getModelType().continuousTime()) {
			if (exprTemp.getBounds().hasStepBounds()) {
				throw new PrismNotSupportedException("Step bounds are not supported for continuous time models");
			}
			if (exprTemp.getBounds().countTimeBoundsContinuous() > 1) {
				throw new PrismNotSupportedException("Multiple time bounds are not supported for continuous time models");
			}
		} else {
			if (exprTemp.getBounds().countStepBounds() > 1) {
				throw new PrismNotSupportedException("Multiple step bounds are not supported by the generic calculator");
			}
		}

		// Negation		
		if (expr instanceof ExpressionUnaryOp &&
		    ((ExpressionUnaryOp)expr).getOperator() == ExpressionUnaryOp.NOT) {
			negated = true;
			minMax = minMax.negate();
			expr = ((ExpressionUnaryOp)expr).getOperand();
		}

		if (expr instanceof ExpressionTemporal) {
 			exprTemp = (ExpressionTemporal) expr;

			// Next
			if (exprTemp.getOperator() == ExpressionTemporal.P_X) {
				probs = checkProbNext(model, exprTemp, minMax, statesOfInterest);
			}
			// Until
			else if (exprTemp.getOperator() == ExpressionTemporal.P_U) {
				if (exprTemp.hasBounds()) {
					probs = checkProbBoundedUntil(model, exprTemp, minMax, statesOfInterest);
				} else {
					probs = checkProbUntil(model, exprTemp, minMax, statesOfInterest);
				}
			}
		}

		if (probs == null)
			throw new PrismException("Unrecognised path operator in P operator");

		if (negated) {
			// Subtract from 1 for negation
			probs.timesConstant(-1.0);
			probs.plusConstant(1.0);
		}

		return probs;
	}

	/**
	 * Compute probabilities for a next operator.
	 */
	protected StateValues checkProbNext(Model model, ExpressionTemporal expr, MinMax minMax, BitSet statesOfInterest) throws PrismException
	{
		if (expr.getBounds().hasRewardBounds()) {
			throw new PrismException("Reachability reward operator with reward bounds is not supported.");
		}

		// Model check the operand for all states
		BitSet target = checkExpression(model, expr.getOperand2(), null).getBitSet();

		// Compute/return the probabilities
		ModelCheckerResult res = null;
		switch (model.getModelType()) {
		case CTMC:
			res = ((CTMCModelChecker) this).computeNextProbs((CTMC) model, target);
			break;
		case DTMC:
			res = ((DTMCModelChecker) this).computeNextProbs((DTMC) model, target);
			break;
		case MDP:
			res = ((MDPModelChecker) this).computeNextProbs((MDP) model, target, minMax.isMin());
			break;
		case STPG:
			res = ((STPGModelChecker) this).computeNextProbs((STPG) model, target, minMax.isMin1(), minMax.isMin2());
			break;
		default:
			throw new PrismNotSupportedException("Cannot model check " + expr + " for " + model.getModelType() + "s");
		}
		result.setStrategy(res.strat);
		return StateValues.createFromDoubleArray(res.soln, model);
	}

	/**
	 * Compute probabilities for a bounded until operator.
	 */
	protected StateValues checkProbBoundedUntil(Model model, ExpressionTemporal expr, MinMax minMax, BitSet statesOfInterest) throws PrismException
	{
		// This method just handles discrete time
		// Continuous-time model checkers will override this method

		// Get info from bounded until
		Integer lowerBound;
		IntegerBound bounds;
		int i;

		// get and check bounds information
		bounds = IntegerBound.fromExpressionTemporal(expr, constantValues, true);

		// Model check operands for all states
		BitSet remain = checkExpression(model, expr.getOperand1(), null).getBitSet();
		BitSet target = checkExpression(model, expr.getOperand2(), null).getBitSet();

		if (bounds.hasLowerBound()) {
			lowerBound = bounds.getLowestInteger();
		} else {
			lowerBound = 0;
		}

		Integer windowSize = null;  // unbounded

		if (bounds.hasUpperBound()) {
			windowSize = bounds.getHighestInteger() - lowerBound;
		}

		// compute probabilities for Until<=windowSize
		StateValues sv = null;

		if (windowSize == null) {
			// unbounded
			ModelCheckerResult res = null;
			switch (model.getModelType()) {
			case DTMC:
				res = ((DTMCModelChecker) this).computeUntilProbs((DTMC) model, remain, target);
				break;
			case MDP:
				res = ((MDPModelChecker) this).computeUntilProbs((MDP) model, remain, target, minMax.isMin());
				break;
			case STPG:
				res = ((STPGModelChecker) this).computeUntilProbs((STPG) model, remain, target, minMax.isMin1(), minMax.isMin2());
				break;
			default:
				throw new PrismException("Cannot model check " + expr + " for " + model.getModelType() + "s");
			}
			result.setStrategy(res.strat);
			sv = StateValues.createFromDoubleArray(res.soln, model);
		} else if (windowSize == 0) {
			// A trivial case: windowSize=0 (prob is 1 in target states, 0 otherwise)
			sv = StateValues.createFromBitSetAsDoubles(target, model);
		} else {
			// Otherwise: numerical solution
			ModelCheckerResult res = null;

			switch (model.getModelType()) {
			case DTMC:
				res = ((DTMCModelChecker) this).computeBoundedUntilProbs((DTMC) model, remain, target, windowSize);
				break;
			case MDP:
				res = ((MDPModelChecker) this).computeBoundedUntilProbs((MDP) model, remain, target, windowSize, minMax.isMin());
				break;
			case STPG:
				res = ((STPGModelChecker) this).computeBoundedUntilProbs((STPG) model, remain, target, windowSize, minMax.isMin1(), minMax.isMin2());
				break;
			default:
				throw new PrismNotSupportedException("Cannot model check " + expr + " for " + model.getModelType() + "s");
			}
			result.setStrategy(res.strat);
			sv = StateValues.createFromDoubleArray(res.soln, model);
		}

		// perform lowerBound restricted next-step computations to
		// deal with lower bound.
		if (lowerBound > 0) {
			double[] probs = sv.getDoubleArray();

			for (i = 0; i < lowerBound; i++) {
				switch (model.getModelType()) {
				case DTMC:
					probs = ((DTMCModelChecker) this).computeRestrictedNext((DTMC) model, remain, probs);
					break;
				case MDP:
					probs = ((MDPModelChecker) this).computeRestrictedNext((MDP) model, remain, probs, minMax.isMin());
					break;
				case STPG:
					// TODO (JK): Figure out if we can handle lower bounds for STPG in the same way
					throw new PrismNotSupportedException("Lower bounds not yet supported for STPGModelChecker");
				default:
					throw new PrismNotSupportedException("Cannot model check " + expr + " for " + model.getModelType() + "s");
				}
			}

			sv = StateValues.createFromDoubleArray(probs, model);
		}

		return sv;
	}

	/**
	 * Compute probabilities for an (unbounded) until operator.
	 */
	protected StateValues checkProbUntil(Model model, ExpressionTemporal expr, MinMax minMax, BitSet statesOfInterest) throws PrismException
	{
		// Model check operands for all states
		BitSet remain = checkExpression(model, expr.getOperand1(), null).getBitSet();
		BitSet target = checkExpression(model, expr.getOperand2(), null).getBitSet();

		// Compute/return the probabilities
		ModelCheckerResult res = null;
		switch (model.getModelType()) {
		case CTMC:
			res = ((CTMCModelChecker) this).computeUntilProbs((CTMC) model, remain, target);
			break;
		case DTMC:
			res = ((DTMCModelChecker) this).computeUntilProbs((DTMC) model, remain, target);
			break;
		case MDP:
			res = ((MDPModelChecker) this).computeUntilProbs((MDP) model, remain, target, minMax.isMin());
			break;
		case STPG:
			res = ((STPGModelChecker) this).computeUntilProbs((STPG) model, remain, target, minMax.isMin1(), minMax.isMin2());
			break;
		default:
			throw new PrismNotSupportedException("Cannot model check " + expr + " for " + model.getModelType() + "s");
		}
		result.setStrategy(res.strat);
		return StateValues.createFromDoubleArray(res.soln, model);
	}

	/**
	 * Compute probabilities for an LTL path formula
	 */
	protected StateValues checkProbPathFormulaLTL(Model model, Expression expr, boolean qual, MinMax minMax, BitSet statesOfInterest) throws PrismException
	{
		// To be overridden by subclasses
		throw new PrismNotSupportedException("Computation not implemented yet");
	}

	/**
	 * Model check an R operator expression and return the values for all states.
	 */
	protected StateValues checkExpressionReward(Model model, ExpressionReward expr, BitSet statesOfInterest) throws PrismException
	{
		// Use the default semantics for a standalone R operator
		// (i.e. quantification over all strategies, and no game-coalition info)
		return checkExpressionReward(model, expr, true, null, statesOfInterest);
	}
	
	/**
	 * Model check an R operator expression and return the values for all states.
	 */
	protected StateValues checkExpressionReward(Model model, ExpressionReward expr, boolean forAll, Coalition coalition, BitSet statesOfInterest) throws PrismException
	{
		// Get info from R operator
		OpRelOpBound opInfo = expr.getRelopBoundInfo(constantValues);
		MinMax minMax = opInfo.getMinMax(model.getModelType(), forAll);

		// Build rewards
		int r = expr.getRewardStructIndexByIndexObject(modelInfo, constantValues);
		mainLog.println("Building reward structure...");
		Rewards rewards = constructRewards(model, r);

		// Compute rewards
		StateValues rews = checkRewardFormula(model, rewards, expr.getExpression(), minMax, statesOfInterest);

		// Print out rewards
		if (getVerbosity() > 5) {
			mainLog.print("\nRewards (non-zero only) for all states:\n");
			rews.print(mainLog);
		}

		// For =? properties, just return values
		if (opInfo.isNumeric()) {
			return rews;
		}
		// Otherwise, compare against bound to get set of satisfying states
		else {
			BitSet sol = rews.getBitSetFromInterval(opInfo.getRelOp(), opInfo.getBound());
			rews.clear();
			return StateValues.createFromBitSet(sol, model);
		}
	}

	/**
	 * Construct rewards for the reward structure with index r of the model generator and a model.
	 * Ensures non-negative rewards.
	 * <br>
	 * Note: Relies on the stored ModelGenerator for constructing the reward structure.
	 */
	protected Rewards constructRewards(Model model, int r) throws PrismException
	{
		return constructRewards(model, r, false);
	}

	/**
	 * Construct rewards for the reward structure with index r of the model generator and a model.
	 * <br>
	 * If {@code allowNegativeRewards} is true, the rewards may be positive and negative, i.e., weights.
	 * <br>
	 * Note: Relies on the stored ModelGenerator for constructing the reward structure.
	 */
	protected Rewards constructRewards(Model model, int r, boolean allowNegativeRewards) throws PrismException
	{
		ConstructRewards constructRewards = new ConstructRewards(mainLog);
		if (allowNegativeRewards)
			constructRewards.allowNegativeRewards();
		return constructRewards.buildRewardStructure(model, modelGen, r);
	}

	/**
	 * Construct rewards from a (non-negative) reward structure and a model.
	 * <br>
	 * Note: Deprecated, use the methods with reward structure index r instead
	 * to allow construction from model generators.
	 */
	@Deprecated
	protected Rewards constructRewards(Model model, RewardStruct rewStruct) throws PrismException
	{
		return constructRewards(model, rewStruct, false);
	}

	/**
	 * Construct rewards from a reward structure and a model.
	 * <br>
	 * If {@code allowNegativeRewards} is true, the rewards may be positive and negative, i.e., weights.
	 * <br>
	 * Note: Deprecated, use the methods with reward structure index r instead
	 * to allow construction from model generators.
	 */
	@Deprecated
	protected Rewards constructRewards(Model model, RewardStruct rewStruct, boolean allowNegativeRewards) throws PrismException
	{
		Rewards rewards;
		ConstructRewards constructRewards = new ConstructRewards(mainLog);

		if (allowNegativeRewards)
			constructRewards.allowNegativeRewards();

		switch (model.getModelType()) {
		case CTMC:
		case DTMC:
			rewards = constructRewards.buildMCRewardStructure((DTMC) model, rewStruct, constantValues);
			break;
		case MDP:
			rewards = constructRewards.buildMDPRewardStructure((MDP) model, rewStruct, constantValues);
			break;
		default:
			throw new PrismNotSupportedException("Cannot build rewards for " + model.getModelType() + "s");
		}
		return rewards;
	}

	/**
	 * Compute rewards for the contents of an R operator.
	 */
	protected StateValues checkRewardFormula(Model model, Rewards modelRewards, Expression expr, MinMax minMax, BitSet statesOfInterest) throws PrismException
	{
		StateValues rewards = null;

		if (expr.getType() instanceof TypePathDouble) {
			ExpressionTemporal exprTemp = (ExpressionTemporal) expr;
			switch (exprTemp.getOperator()) {
			case ExpressionTemporal.R_I:
				rewards = checkRewardInstantaneous(model, modelRewards, exprTemp, minMax);
				break;
			case ExpressionTemporal.R_C:
				if (exprTemp.hasBounds()) {
					rewards = checkRewardCumulative(model, modelRewards, exprTemp, minMax);
				} else {
					rewards = checkRewardTotal(model, modelRewards, exprTemp, minMax);
				}
				break;
			default:
				throw new PrismNotSupportedException("Explicit engine does not yet handle the " + exprTemp.getOperatorSymbol() + " reward operator");
			}
		} else if (expr.getType() instanceof TypePathBool || expr.getType() instanceof TypeBool) {
			rewards = checkRewardPathFormula(model, modelRewards, expr, minMax, statesOfInterest);
		}

		if (rewards == null)
			throw new PrismException("Unrecognised operator in R operator");

		return rewards;
	}

	/**
	 * Compute rewards for an instantaneous reward operator.
	 */
	protected StateValues checkRewardInstantaneous(Model model, Rewards modelRewards, ExpressionTemporal expr, MinMax minMax) throws PrismException
	{
		if (expr.getBounds().hasRewardBounds()) {
			throw new PrismNotSupportedException("Instantaneous reward operator with reward bounds is not supported.");
		}

		// Get time bound
		double t = expr.getBounds().getStandardBound(model.getModelType()).getUpperBound().evaluateDouble(constantValues);

		// Compute/return the rewards
		ModelCheckerResult res = null;
		switch (model.getModelType()) {
		case DTMC:
			res = ((DTMCModelChecker) this).computeInstantaneousRewards((DTMC) model, (MCRewards) modelRewards, t);
			break;
		case CTMC:
			res = ((CTMCModelChecker) this).computeInstantaneousRewards((CTMC) model, (MCRewards) modelRewards, t);
			break;
		case MDP:
			res = ((MDPModelChecker) this).computeInstantaneousRewards((MDP) model, (MDPRewards) modelRewards, t, minMax.isMin());
			break;
		default:
			throw new PrismNotSupportedException("Explicit engine does not yet handle the " + expr.getOperatorSymbol() + " reward operator for " + model.getModelType()
					+ "s");
		}
		result.setStrategy(res.strat);
		return StateValues.createFromDoubleArray(res.soln, model);
	}

	/**
	 * Compute rewards for a cumulative reward operator.
	 */
	protected StateValues checkRewardCumulative(Model model, Rewards modelRewards, ExpressionTemporal expr, MinMax minMax) throws PrismException
	{
		int timeInt = -1;
		double timeDouble = -1;

		if (expr.getBounds().hasRewardBounds()) {
			throw new PrismNotSupportedException("Cumulative reward operator with reward bounds is not supported.");
		}

		TemporalOperatorBound bound = expr.getBounds().getStandardBound(model.getModelType());
		if (bound == null || bound.getUpperBound() == null) {
			throw new PrismException("This is not a cumulative reward operator");
		}

		// Get time bound
		if (model.getModelType().continuousTime()) {
			timeDouble = bound.getUpperBound().evaluateDouble(constantValues);
			if (timeDouble < 0) {
				throw new PrismException("Invalid time bound " + timeDouble + " in cumulative reward formula");
			}
		} else {
			timeInt = bound.getUpperBound().evaluateInt(constantValues);
			if (timeInt < 0) {
				throw new PrismException("Invalid time bound " + timeInt + " in cumulative reward formula");
			}
		}

		// Compute/return the rewards
		// A trivial case: "C<=0" (prob is 1 in target states, 0 otherwise)
		if (timeInt == 0 || timeDouble == 0) {
			return new StateValues(TypeDouble.getInstance(), model.getNumStates(), new Double(0));
		}
		// Otherwise: numerical solution
		ModelCheckerResult res = null;
		switch (model.getModelType()) {
		case DTMC:
			res = ((DTMCModelChecker) this).computeCumulativeRewards((DTMC) model, (MCRewards) modelRewards, timeInt);
			break;
		case CTMC:
			res = ((CTMCModelChecker) this).computeCumulativeRewards((CTMC) model, (MCRewards) modelRewards, timeDouble);
			break;
		case MDP:
			res = ((MDPModelChecker) this).computeCumulativeRewards((MDP) model, (MDPRewards) modelRewards, timeInt, minMax.isMin());
			break;
		default:
			throw new PrismNotSupportedException("Explicit engine does not yet handle the " + expr.getOperatorSymbol() + " reward operator for " + model.getModelType()
					+ "s");
		}
		result.setStrategy(res.strat);
		return StateValues.createFromDoubleArray(res.soln, model);
	}

	/**
	 * Compute expected rewards for a total reward operator.
	 */
	protected StateValues checkRewardTotal(Model model, Rewards modelRewards, ExpressionTemporal expr, MinMax minMax) throws PrismException
	{
		if (expr.getBounds().hasRewardBounds()) {
			throw new PrismNotSupportedException("Cumulative reward operator with reward bounds is not supported.");
		}

		if (expr.hasBounds()) {
			throw new PrismException("This is not a total reward operator");
		}

		// Compute/return the rewards
		ModelCheckerResult res = null;
		switch (model.getModelType()) {
		case DTMC:
			res = ((DTMCModelChecker) this).computeTotalRewards((DTMC) model, (MCRewards) modelRewards);
			break;
		case CTMC:
			res = ((CTMCModelChecker) this).computeTotalRewards((CTMC) model, (MCRewards) modelRewards);
			break;
		case MDP:
			res = ((MDPModelChecker) this).computeTotalRewards((MDP) model, (MDPRewards) modelRewards, minMax.isMin());
			break;
		default:
			throw new PrismNotSupportedException("Explicit engine does not yet handle the " + expr.getOperatorSymbol() + " reward operator for " + model.getModelType()
					+ "s");
		}
		result.setStrategy(res.strat);
		return StateValues.createFromDoubleArray(res.soln, model);
	}

	/**
	 * Compute rewards for a path formula in a reward operator.
	 */
	protected StateValues checkRewardPathFormula(Model model, Rewards modelRewards, Expression expr, MinMax minMax, BitSet statesOfInterest) throws PrismException
	{
		if (Expression.isReach(expr)) {
			return checkRewardReach(model, modelRewards, (ExpressionTemporal) expr, minMax, statesOfInterest);
		}
		else if (Expression.isCoSafeLTLSyntactic(expr, true)) {
			return checkRewardCoSafeLTL(model, modelRewards, expr, minMax, statesOfInterest);
		}
		throw new PrismException("R operator contains a path formula that is not syntactically co-safe: " + expr);
	}
	
	/**
	 * Compute rewards for a reachability reward operator.
	 */
	protected StateValues checkRewardReach(Model model, Rewards modelRewards, ExpressionTemporal expr, MinMax minMax, BitSet statesOfInterest) throws PrismException
	{
		// No time bounds allowed
		if (expr.hasBounds()) {
			throw new PrismNotSupportedException("R operator cannot contain a bounded F operator: " + expr);
		}
		
		// Model check the operand for all states
		BitSet target = checkExpression(model, expr.getOperand2(), null).getBitSet();

		// Compute/return the rewards
		ModelCheckerResult res = null;
		switch (model.getModelType()) {
		case DTMC:
			res = ((DTMCModelChecker) this).computeReachRewards((DTMC) model, (MCRewards) modelRewards, target);
			break;
		case CTMC:
			res = ((CTMCModelChecker) this).computeReachRewards((CTMC) model, (MCRewards) modelRewards, target);
			break;
		case MDP:
			res = ((MDPModelChecker) this).computeReachRewards((MDP) model, (MDPRewards) modelRewards, target, minMax.isMin());
			break;
		case STPG:
			res = ((STPGModelChecker) this).computeReachRewards((STPG) model, (STPGRewards) modelRewards, target, minMax.isMin1(), minMax.isMin2());
			break;
		default:
			throw new PrismNotSupportedException("Explicit engine does not yet handle the " + expr.getOperatorSymbol() + " reward operator for " + model.getModelType()
					+ "s");
		}
		result.setStrategy(res.strat);
		return StateValues.createFromDoubleArray(res.soln, model);
	}

	/**
	 * Compute rewards for a co-safe LTL reward operator.
	 */
	protected StateValues checkRewardCoSafeLTL(Model model, Rewards modelRewards, Expression expr, MinMax minMax, BitSet statesOfInterest) throws PrismException
	{
		// To be overridden by subclasses
		throw new PrismException("Computation not implemented yet");
	}

	/**
	 * Model check an S operator expression and return the values for all states.
	 */
	protected StateValues checkExpressionSteadyState(Model model, ExpressionSS expr) throws PrismException
	{
		// Get info from S operator
		OpRelOpBound opInfo = expr.getRelopBoundInfo(constantValues);
		MinMax minMax = opInfo.getMinMax(model.getModelType());

		// Compute probabilities
		StateValues probs = checkSteadyStateFormula(model, expr.getExpression(), minMax);

		// Print out probabilities
		if (getVerbosity() > 5) {
			mainLog.print("\nProbabilities (non-zero only) for all states:\n");
			probs.print(mainLog);
		}

		// For =? properties, just return values
		if (opInfo.isNumeric()) {
			return probs;
		}
		// Otherwise, compare against bound to get set of satisfying states
		else {
			BitSet sol = probs.getBitSetFromInterval(opInfo.getRelOp(), opInfo.getBound());
			probs.clear();
			return StateValues.createFromBitSet(sol, model);
		}
	}

	/**
	 * Compute steady-state probabilities for an S operator.
	 */
	protected StateValues checkSteadyStateFormula(Model model, Expression expr, MinMax minMax) throws PrismException
	{
		// Model check operand for all states
		BitSet b = checkExpression(model, expr, null).getBitSet();

		// Compute/return the probabilities
		ModelCheckerResult res = null;
		switch (model.getModelType()) {
		case DTMC:
			double multProbs[] = Utils.bitsetToDoubleArray(b, model.getNumStates());
			res = ((DTMCModelChecker) this).computeSteadyStateBackwardsProbs((DTMC) model, multProbs);
			break;
		default:
			throw new PrismNotSupportedException("Explicit engine does not yet handle the S operator for " + model.getModelType() + "s");
		}
		return StateValues.createFromDoubleArray(res.soln, model);
	}

	// Utility methods for probability distributions

	/**
	 * Generate a probability distribution, stored as a StateValues object, from a file.
	 * If {@code distFile} is null, so is the return value.
	 */
	public StateValues readDistributionFromFile(File distFile, Model model) throws PrismException
	{
		StateValues dist = null;

		if (distFile != null) {
			mainLog.println("\nImporting probability distribution from file \"" + distFile + "\"...");
			// Build an empty vector 
			dist = new StateValues(TypeDouble.getInstance(), model);
			// Populate vector from file
			dist.readFromFile(distFile);
		}

		return dist;
	}

	/**
	 * Build a probability distribution, stored as a StateValues object,
	 * from the initial states info of the current model: either probability 1 for
	 * the (single) initial state or equiprobable over multiple initial states.
	 */
	public StateValues buildInitialDistribution(Model model) throws PrismException
	{
		StateValues dist = null;

		// Build an empty vector 
		dist = new StateValues(TypeDouble.getInstance(), model);
		// Populate vector (equiprobable over initial states)
		double d = 1.0 / model.getNumInitialStates();
		for (int in : model.getInitialStates()) {
			dist.setDoubleValue(in, d);
		}

		return dist;
	}

	protected static List<TemporalOperatorBound> getBoundsForReplacement(final TemporalOperatorBounds temporalOperatorBounds, final boolean useCounterForAllBounds) throws PrismLangException
	{
		final List<TemporalOperatorBound> boundsToReplace = new ArrayList<>();
		if (useCounterForAllBounds){
			boundsToReplace.addAll(temporalOperatorBounds.getRewardBounds());
			boundsToReplace.addAll(temporalOperatorBounds.getStepBoundsForDiscreteTime());
			return boundsToReplace;
		}
		final int numberOfRewardBounds = temporalOperatorBounds.countRewardBounds();
		final int numberOfTimeBounds = temporalOperatorBounds.countTimeBoundsDiscrete();
		if (numberOfRewardBounds + numberOfTimeBounds >= 2){
			//at least one bound needs to be replaced by counters ...
			if (numberOfRewardBounds > 0){
				boundsToReplace.addAll(temporalOperatorBounds.getRewardBounds());
				//calculate very first reward bound using quantile-backend
				boundsToReplace.remove(0);
				//at least one reward bound ==> use quantile-backend
				// ==> replace each time-bound using counters
				boundsToReplace.addAll(temporalOperatorBounds.getStepBoundsForDiscreteTime());
				return boundsToReplace;
			}
			assert (numberOfRewardBounds == 0);
			assert (numberOfTimeBounds >= 2);
			boundsToReplace.addAll(temporalOperatorBounds.getStepBoundsForDiscreteTime());
			//one time bound can be calculated either by standard simple path formula procedure or by quantile-backend
			boundsToReplace.remove(0);
			return boundsToReplace;
		}
		assert (numberOfRewardBounds + numberOfTimeBounds <= 1);
		//since there is at most 1 bound given, there is no need to replace any bound
		return boundsToReplace;
	}
}
