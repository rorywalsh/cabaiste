/*
  ==============================================================================

    ComponentLayoutEditor.h
    Created: 26 Oct 2016 9:18:16pm
    Author:  rory

  ==============================================================================
*/

#ifndef COMPONENTLAYOUTEDITOR_H_INCLUDED
#define COMPONENTLAYOUTEDITOR_H_INCLUDED


/*
 *  ComponentLayoutEditor.h
 *  
 *  Original written by Haydxn
 *  Modified by Jordan Hochenbaum on 10/25/10.
 *  http://www.rawmaterialsoftware.com/viewtopic.php?f=6&t=2635
 *
 */

#include "../CabbageCommonHeaders.h"
 
//=============================================================================
class ChildAlias   :   public Component
    {
    public:
		ChildAlias (Component* targetChild);
		~ChildAlias ();
		void resized ();
		void paint (Graphics& g);
		const Component* getTargetChild ();
		void updateFromTarget ();
		void applyToTarget ();
		virtual void userChangedBounds ();
		virtual void userStartedChangingBounds ();
		virtual void userStoppedChangingBounds ();
		bool boundsChangedSinceStart ();
		void mouseEnter (const MouseEvent& e);
		void mouseExit (const MouseEvent& e);
		void mouseDown (const MouseEvent& e);
		void mouseUp (const MouseEvent& e);
		void mouseDrag (const MouseEvent& e);
		
    private:
		CriticalSection bounds;
		ScopedPointer<ComponentBoundsConstrainer>  constrainer;
    	ComponentDragger dragger;
		SafePointer<Component> target;
		bool interest;
		bool userAdjusting;
		Rectangle<int> startBounds;
		ScopedPointer<ComponentBoundsConstrainer> resizeContainer; //added resizeContainer to limit resizing sizes
		ResizableBorderComponent* resizer;
};
//=============================================================================
class ComponentLayoutEditor   :   public Component
    {
    public:
		enum ColourIds
		{
			aliasIdleColour,
			aliasHoverColour
		};
		ComponentLayoutEditor ();
		~ComponentLayoutEditor ();
		void resized ();
		void paint (Graphics& g);
		void setTargetComponent (Component* target);
		void bindWithTarget ();
		void updateFrames ();
    	void enablementChanged ();
	const Component* getTarget ();
	
private:
	
	virtual ChildAlias* createAlias (Component* child);
	
	SafePointer<Component> target;
	OwnedArray<ChildAlias> frames;
	
};


#endif  // COMPONENTLAYOUTEDITOR_H_INCLUDED
