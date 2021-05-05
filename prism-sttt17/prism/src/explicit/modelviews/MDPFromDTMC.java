package explicit.modelviews;

import java.util.BitSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map.Entry;
import java.util.Set;

import explicit.DTMC;
import explicit.DTMCSimple;
import explicit.MDP;
import parser.State;
import parser.Values;
import parser.VarList;
import prism.PrismException;

public class MDPFromDTMC extends MDPView
{
	private DTMC model;



	public MDPFromDTMC(final DTMC model)
	{
		this.model = model;
	}

	public MDPFromDTMC(final MDPFromDTMC mdp)
	{
		super(mdp);
		model = mdp.model;
	}



	//--- Cloneable ---

	@Override
	public MDPFromDTMC clone()
	{
		return new MDPFromDTMC(this);
	}



	//--- Model ---

	@Override
	public int getNumStates()
	{
		return model.getNumStates();
	}

	@Override
	public int getNumInitialStates()
	{
		return model.getNumInitialStates();
	}

	@Override
	public Iterable<Integer> getInitialStates()
	{
		return model.getInitialStates();
	}

	@Override
	public int getFirstInitialState()
	{
		return model.getFirstInitialState();
	}

	@Override
	public boolean isInitialState(final int state)
	{
		return model.isInitialState(state);
	}

	@Override
	public List<State> getStatesList()
	{
		return model.getStatesList();
	}

	@Override
	public VarList getVarList()
	{
		return model.getVarList();
	}

	@Override
	public Values getConstantValues()
	{
		return model.getConstantValues();
	}

	@Override
	public BitSet getLabelStates(final String name)
	{
		return model.getLabelStates(name);
	}

	@Override
	public Set<String> getLabels()
	{
		return model.getLabels();
	}

	@Override
	public boolean hasLabel(String name)
	{
		return model.hasLabel(name);
	}


	@Override
	public Iterator<Integer> getSuccessorsIterator(final int state)
	{
		return model.getSuccessorsIterator(state);
	}



	//--- NondetModel ---

	@Override
	public int getNumChoices(final int state)
	{
		return model.getTransitionsIterator(state).hasNext() ? 1 : 0;
	}

	@Override
	public Object getAction(final int state, final int choice)
	{
		if (choice >= getNumChoices(state)) {
			throw new IndexOutOfBoundsException("choice index out of bounds");
		}
		return null;
	}

	@Override
	public boolean areAllChoiceActionsUnique()
	{
		return true;
	}

	@Override
	public Iterator<Integer> getSuccessorsIterator(final int state, final int choice)
	{
		if (choice >= getNumChoices(state)) {
			throw new IndexOutOfBoundsException("choice index out of bounds");
		}
		return model.getSuccessorsIterator(state);
	}



	//--- MDP ---

	@Override
	public Iterator<Entry<Integer, Double>> getTransitionsIterator(final int state, final int choice)
	{
		if (choice > 0) {
			throw new IndexOutOfBoundsException("choice index out of bounds");
		}
		final Iterator<Entry<Integer, Double>> transitions = model.getTransitionsIterator(state);
		if (!transitions.hasNext()) {
			throw new IndexOutOfBoundsException("choice index out of bounds");
		}
		return transitions;
	}



	//--- MDPView ---

	@Override
	protected void fixDeadlocks()
	{
		assert !fixedDeadlocks : "deadlocks already fixed";

		try {
			model.findDeadlocks(false);
		} catch (final PrismException e) {
			assert false : "no attempt to fix deadlocks";
		}
		model = DTMCAlteredDistributions.fixDeadlocks(model);
	}



	//--- static methods ---

	public static void main(final String[] args) throws PrismException
	{
		final DTMCSimple original = new DTMCSimple(4);
		original.addInitialState(1);
		original.setProbability(0, 1, 0.1);
		original.setProbability(0, 2, 0.9);
		original.setProbability(1, 2, 0.2);
		original.setProbability(1, 3, 0.8);
		original.setProbability(2, 1, 0.3);
		original.setProbability(2, 2, 0.7);
		original.findDeadlocks(false);
		System.out.println(original);

		final MDP mdp = new MDPFromDTMC(original);
		mdp.findDeadlocks(true);
		System.out.println(mdp);
	}
}