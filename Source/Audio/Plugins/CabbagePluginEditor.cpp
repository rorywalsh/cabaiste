/*
  Copyright (C) 2016 Rory Walsh

  Cabbage is free software; you can redistribute it
  and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  Cabbage is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU General Public
  License along with Csound; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
  02111-1307 USA
*/
#include "CabbagePluginEditor.h"

class CabbageCheckbox;

//==============================================================================
CabbagePluginEditor::CabbagePluginEditor (CabbagePluginProcessor& p)
    : AudioProcessorEditor (&p),
      processor (p),
      lookAndFeel(),
      layoutEditor(processor.cabbageWidgets),
      mainComponent()
{
    setName("PluginEditor");
    setSize (400, 300);
    setLookAndFeel(&lookAndFeel);
    createEditorInterface(processor.cabbageWidgets);
    addAndMakeVisible(layoutEditor);
    addAndMakeVisible(mainComponent);
    mainComponent.setInterceptsMouseClicks(false, true);
    layoutEditor.setTargetComponent(&mainComponent);
    layoutEditor.updateFrames();
    layoutEditor.setEnabled(false);
    layoutEditor.toFront(false);
    layoutEditor.setInterceptsMouseClicks(true, true);
}

CabbagePluginEditor::~CabbagePluginEditor()
{

}

//==============================================================================
void CabbagePluginEditor::paint (Graphics& g)
{
    g.fillAll (backgroundColour);
}

void CabbagePluginEditor::resized()
{
    layoutEditor.setBounds(getLocalBounds());
    mainComponent.setBounds(getLocalBounds());
}
//==============================================================================
void CabbagePluginEditor::createEditorInterface(ValueTree widgets)
{
    components.clear();

    for(int widget=0; widget<widgets.getNumChildren(); widget++)
    {
        const String widgetType = widgets.getChild(widget).getProperty(CabbageIdentifierIds::type).toString();

        if(widgetType==CabbageIdentifierIds::form)
            setupWindow(widgets.getChild(widget));
        else
        {
            insertWidget(widgets.getChild(widget));
        }
    }
}

//======================================================================================================
void CabbagePluginEditor::setupWindow(ValueTree widgetData)
{
    const String name = CabbageWidgetData::getStringProp(widgetData, CabbageIdentifierIds::caption);
    setName(CabbageWidgetData::getStringProp(widgetData, CabbageIdentifierIds::caption));
    const int width = CabbageWidgetData::getNumProp(widgetData, CabbageIdentifierIds::width);
    const int height = CabbageWidgetData::getNumProp(widgetData, CabbageIdentifierIds::height);
    const String backgroundColourString = CabbageWidgetData::getStringProp(widgetData, CabbageIdentifierIds::colour);
    backgroundColour = Colour::fromString(backgroundColourString);
    setSize(width, height);
    repaint();
}
//======================================================================================================
void CabbagePluginEditor::addNewWidget(String widgetType, Point<int> position)
{

    StringArray csdArray = processor.getCurrentCsdFileAsStringArray();
    const String widgetTreeIdentifier = "newlyAddedWidget";
    ValueTree newWidget(widgetTreeIdentifier);

    CabbageWidgetData::setWidgetState(newWidget, widgetType, newlyAddedWidgetIndex);
    newWidget.setProperty(CabbageIdentifierIds::top, position.getY(), 0);
    newWidget.setProperty(CabbageIdentifierIds::left, position.getX(), 0);

    processor.cabbageWidgets.addChild(newWidget, -1, 0);

    setCurrentlySelectedComponents(StringArray(CabbageWidgetData::getStringProp(newWidget, CabbageIdentifierIds::name)));

    insertWidget(newWidget);
    updateLayoutEditorFrames();

    sendChangeMessage(); 	//update code in editor

    newlyAddedWidgetIndex++;

}

//======================================================================================================
void CabbagePluginEditor::insertWidget(ValueTree cabbageWidgetData)
{
    const String widgetType = cabbageWidgetData.getProperty(CabbageIdentifierIds::type).toString();

    if(widgetType==CabbageIdentifierIds::checkbox)
        insertCheckbox(cabbageWidgetData);

    else if(widgetType==CabbageIdentifierIds::combobox)
        insertComboBox(cabbageWidgetData);

    else if(widgetType==CabbageIdentifierIds::image)
        insertImage(cabbageWidgetData);

    else if(widgetType==CabbageIdentifierIds::rslider
            || widgetType==CabbageIdentifierIds::vslider
            || widgetType==CabbageIdentifierIds::hslider)
        insertSlider(cabbageWidgetData);

    else if(widgetType==CabbageIdentifierIds::label)
        insertLabel(cabbageWidgetData);

    else if(widgetType==CabbageIdentifierIds::groupbox)
        insertGroupBox(cabbageWidgetData);

    else if(widgetType==CabbageIdentifierIds::keyboard)
        insertMIDIKeyboard(cabbageWidgetData);

    else if(widgetType==CabbageIdentifierIds::csoundoutput)
        insertCsoundOutputConsole(cabbageWidgetData);

    else if(widgetType==CabbageIdentifierIds::numberbox)
        insertNumberBox(cabbageWidgetData);

    else if(widgetType==CabbageIdentifierIds::textbox.toString())
        insertTextBox(cabbageWidgetData);

    else if(widgetType==CabbageIdentifierIds::texteditor)
        insertTextEditor(cabbageWidgetData);

    else if(widgetType==CabbageIdentifierIds::encoder)
        insertEncoder(cabbageWidgetData);

    else if(widgetType==CabbageIdentifierIds::soundfiler)
        insertSoundfiler(cabbageWidgetData);

    else if(widgetType==CabbageIdentifierIds::button)
        insertButton(cabbageWidgetData);

    else if(widgetType==CabbageIdentifierIds::filebutton)
        insertFileButton(cabbageWidgetData);

    else if(widgetType==CabbageIdentifierIds::infobutton)
        insertInfoButton(cabbageWidgetData);

    else if(widgetType==CabbageIdentifierIds::signaldisplay)
        insertSignalDisplay(cabbageWidgetData);

    else if(widgetType==CabbageIdentifierIds::gentable)
        insertGenTable(cabbageWidgetData);

    else if(widgetType==CabbageIdentifierIds::xypad)
        insertXYPad(cabbageWidgetData);

    else if(widgetType==CabbageIdentifierIds::hrange
            || widgetType==CabbageIdentifierIds::vrange)
        insertRangeSlider(cabbageWidgetData);
}

void CabbagePluginEditor::insertCheckbox(ValueTree cabbageWidgetData)
{
    CabbageCheckbox* checkbox;
    components.add(checkbox = new CabbageCheckbox(cabbageWidgetData));
    checkbox->addListener(this);
    addToEditorAndMakeVisible(checkbox, cabbageWidgetData);
}

void CabbagePluginEditor::insertComboBox(ValueTree cabbageWidgetData)
{
    CabbageComboBox* combobox;
    components.add(combobox = new CabbageComboBox(cabbageWidgetData, this));
    combobox->addListener(this);
    addToEditorAndMakeVisible(combobox, cabbageWidgetData);
}

void CabbagePluginEditor::insertRangeSlider(ValueTree cabbageWidgetData)
{
    CabbageRangeSlider* rangeSlider;
    components.add(rangeSlider = new CabbageRangeSlider(cabbageWidgetData, this));
    rangeSlider->getSlider().addListener(this);
    addToEditorAndMakeVisible(rangeSlider, cabbageWidgetData);
}

void CabbagePluginEditor::insertImage(ValueTree cabbageWidgetData)
{
    CabbageImage* image;
    components.add(image = new CabbageImage(cabbageWidgetData, this));
    addToEditorAndMakeVisible(image, cabbageWidgetData);
}

void CabbagePluginEditor::insertLabel(ValueTree cabbageWidgetData)
{
    CabbageLabel* label;
    components.add(label = new CabbageLabel(cabbageWidgetData, this));
    addToEditorAndMakeVisible(label, cabbageWidgetData);
}

void CabbagePluginEditor::insertTextEditor(ValueTree cabbageWidgetData)
{
    CabbageTextEditor* editor;
    components.add(editor = new CabbageTextEditor(cabbageWidgetData, this));
    addToEditorAndMakeVisible(editor, cabbageWidgetData);
}

void CabbagePluginEditor::insertSoundfiler(ValueTree cabbageWidgetData)
{
    CabbageSoundfiler* soundfiler;
    components.add(soundfiler = new CabbageSoundfiler(cabbageWidgetData, this));
    addToEditorAndMakeVisible(soundfiler, cabbageWidgetData);
}

void CabbagePluginEditor::insertSlider(ValueTree cabbageWidgetData)
{
    CabbageSlider* slider;
    components.add(slider = new CabbageSlider(cabbageWidgetData, this));
    slider->getSlider().addListener(this);
    addToEditorAndMakeVisible(slider, cabbageWidgetData);
}

void CabbagePluginEditor::insertEncoder(ValueTree cabbageWidgetData)
{
    CabbageEncoder* encoder;
    components.add(encoder = new CabbageEncoder(cabbageWidgetData, this));
    addToEditorAndMakeVisible(encoder, cabbageWidgetData);
}

void CabbagePluginEditor::insertXYPad(ValueTree cabbageWidgetData)
{
    CabbageXYPad* xyPad;
    components.add(xyPad = new CabbageXYPad(cabbageWidgetData, this));
    xyPad->getSliderX().addListener(this);
    xyPad->getSliderY().addListener(this);
    addToEditorAndMakeVisible(xyPad, cabbageWidgetData);
    processor.addXYAutomator(xyPad, cabbageWidgetData);

}

void CabbagePluginEditor::insertFileButton(ValueTree cabbageWidgetData)
{
    CabbageFileButton* fileButton;
    components.add(fileButton = new CabbageFileButton(cabbageWidgetData, this));
    addToEditorAndMakeVisible(fileButton, cabbageWidgetData);
}

void CabbagePluginEditor::insertSignalDisplay(ValueTree cabbageWidgetData)
{
    CabbageSignalDisplay* signalDisplay;
    components.add(signalDisplay = new CabbageSignalDisplay(cabbageWidgetData, this));
    addToEditorAndMakeVisible(signalDisplay, cabbageWidgetData);
}

void CabbagePluginEditor::insertInfoButton(ValueTree cabbageWidgetData)
{
    CabbageInfoButton* infoButton;
    components.add(infoButton = new CabbageInfoButton(cabbageWidgetData));
    addToEditorAndMakeVisible(infoButton, cabbageWidgetData);
}

void CabbagePluginEditor::insertButton(ValueTree cabbageWidgetData)
{
    CabbageButton* button;
    components.add(button = new CabbageButton(cabbageWidgetData));
    button->addListener(this);
    addToEditorAndMakeVisible(button, cabbageWidgetData);
}

void CabbagePluginEditor::insertNumberBox(ValueTree cabbageWidgetData)
{
    CabbageNumberBox* numberBox;
    components.add(numberBox = new CabbageNumberBox(cabbageWidgetData));
    numberBox->getSlider().addListener(this);
    addToEditorAndMakeVisible(numberBox, cabbageWidgetData);
}

void CabbagePluginEditor::insertGenTable(ValueTree cabbageWidgetData)
{
    CabbageGenTable* genTable;
    components.add(genTable = new CabbageGenTable(cabbageWidgetData, this));
    addToEditorAndMakeVisible(genTable, cabbageWidgetData);
}

void CabbagePluginEditor::insertGroupBox(ValueTree cabbageWidgetData)
{
    CabbageGroupBox* groupBox;
    components.add(groupBox = new CabbageGroupBox(cabbageWidgetData));
    addToEditorAndMakeVisible(groupBox, cabbageWidgetData);
}

void CabbagePluginEditor::insertTextBox(ValueTree cabbageWidgetData)
{
    CabbageTextBox* textBox;
    components.add(textBox = new CabbageTextBox(cabbageWidgetData));
    addToEditorAndMakeVisible(textBox, cabbageWidgetData);
}

void CabbagePluginEditor::insertCsoundOutputConsole(ValueTree cabbageWidgetData)
{
    if(consoleCount<1)
    {
        CabbageCsoundConsole* csoundConsole;
        components.add(csoundConsole = new CabbageCsoundConsole(cabbageWidgetData, this));
        addToEditorAndMakeVisible(csoundConsole, cabbageWidgetData);
        csoundConsole++;
    }
}

void CabbagePluginEditor::insertMIDIKeyboard(ValueTree cabbageWidgetData)
{
    if(keyboardCount<1)
    {
        CabbageKeyboard* midiKeyboard;
        components.add(midiKeyboard = new CabbageKeyboard(cabbageWidgetData, processor.keyboardState));
        midiKeyboard->setKeyPressBaseOctave(3);
        addToEditorAndMakeVisible(midiKeyboard, cabbageWidgetData);
        keyboardCount++;
    }
}
//======================================================================================================
CabbageAudioParameter* CabbagePluginEditor::getParameterForComponent (const String name)
{
	for(auto param : processor.getParameters())
	{
		if(CabbageAudioParameter* cabbageParam = dynamic_cast<CabbageAudioParameter*>(param))
		{
        if(name==cabbageParam->getWidgetName())
            return dynamic_cast<CabbageAudioParameter*> (cabbageParam);
			
		}
	}

    return nullptr;
}

//======================================================================================================
void CabbagePluginEditor::comboBoxChanged (ComboBox* combo)
{
    if (CabbageAudioParameter* param = getParameterForComponent(combo->getName()))
    {
        param->beginChangeGesture();
        const int value = combo->getSelectedItemIndex()+1;
        param->setValueNotifyingHost(value);
        param->endChangeGesture();
    }
}
//======================================================================================================
void CabbagePluginEditor::buttonClicked(Button* button)
{

    const bool buttonState = button->getToggleState();

    if (CabbageAudioParameter* param = getParameterForComponent(button->getName()))	//only update parameters for normal buttons
    {
        param->beginChangeGesture();
        param->setValueNotifyingHost(buttonState == true ? 1 : 0);
        param->endChangeGesture();
    }

    if(CabbageButton* cabbageButton = dynamic_cast<CabbageButton*>(button))
    {
        const StringArray textItems = cabbageButton->getTextArray();
        if(textItems.size()>0)
            cabbageButton->setButtonText( textItems[ buttonState == 0 ? 1 : 0]);
    }
}

//======================================================================================================
void CabbagePluginEditor::sliderValueChanged(Slider* slider)
{
    if(slider->getSliderStyle()!=Slider::TwoValueHorizontal && slider->getSliderStyle()!=Slider::TwoValueVertical)
    {
        if (CabbageAudioParameter* param = getParameterForComponent(slider->getName()))
        {
            param->beginChangeGesture();
            param->setValueNotifyingHost(slider->getValue());
            param->endChangeGesture();
        }
    }
    else
    {
        if (CabbageAudioParameter* param = getParameterForComponent(slider->getName()+"_min"))
        {
            param->beginChangeGesture();
            param->setValueNotifyingHost(slider->getValue());
            param->endChangeGesture();
        }

        if (CabbageAudioParameter* param = getParameterForComponent(slider->getName()+"_max"))
        {
            param->beginChangeGesture();
            param->setValueNotifyingHost(slider->getValue());
            param->endChangeGesture();
        }
    }
}
//======================================================================================================
void CabbagePluginEditor::enableEditMode(bool enable)
{
    layoutEditor.setEnabled(enable);
    editModeEnabled = enable;
    layoutEditor.toFront(false);
}
//======================================================================================================
void CabbagePluginEditor::setCurrentlySelectedComponents(StringArray componentNames)
{
    currentlySelectedComponentNames = componentNames;
}

void CabbagePluginEditor::resetCurrentlySelectedComponents()
{
    currentlySelectedComponentNames.clear();
}

Component* CabbagePluginEditor::getComponentFromName(String name)
{
    for (auto comp : components)
    {
        if(name == comp->getName())
            return comp;
    }

    return nullptr;
}

Array<ValueTree> CabbagePluginEditor::getValueTreesForCurrentlySelectedComponents()
{
    Array<ValueTree> valueTreeArray;
    for(String compName : currentlySelectedComponentNames)
        valueTreeArray.add(CabbageWidgetData::getValueTreeForComponent(processor.cabbageWidgets, compName));

    return valueTreeArray;
}

ValueTree CabbagePluginEditor::getValueTreeForComponent(String compName)
{
    return CabbageWidgetData::getValueTreeForComponent(processor.cabbageWidgets, getComponentFromName(compName)->getName());
}

void CabbagePluginEditor::updateLayoutEditorFrames()
{
    if(editModeEnabled)
        layoutEditor.updateFrames();
}

//======================================================================================================
void CabbagePluginEditor::addToEditorAndMakeVisible(Component* comp, ValueTree widgetData)
{
    const String parent = CabbageWidgetData::getStringProp(widgetData, CabbageIdentifierIds::parentcomponent);
    if(auto parentComp = getComponentFromName(parent))
    {
        parentComp->addAndMakeVisible(comp);
    }
    else
        mainComponent.addAndMakeVisible(comp);
}

//======================================================================================================
void CabbagePluginEditor::updatefTableData(GenTable* table)
{
    Array<double> pFields = table->getPfields();
    if( table->genRoutine==5 || table->genRoutine==7 || table->genRoutine==2)
    {
        FUNC *ftpp;
        EVTBLK  evt;
        memset(&evt, 0, sizeof(EVTBLK));
        evt.pcnt = 5+pFields.size();
        evt.opcod = 'f';
        evt.p[0]=0;

        //setting table number to 0.
        evt.p[1]=0;
        evt.p[2]=0;
        evt.p[3]=table->tableSize;
        evt.p[4]=table->realGenRoutine;
        if(table->genRoutine==5)
        {
            for(int i=0; i<pFields.size()-1; i++)
                evt.p[5+i]= jmax(0.00001, pFields[i+1]);
        }
        else if(table->genRoutine==7)
        {
            for(int i=0; i<pFields.size()-1; i++)
                evt.p[5+i]= pFields[i+1];
        }
        else
        {
            for(int i=0; i<pFields.size(); i++)
                evt.p[5+i] = pFields[i];
        }

        StringArray fStatement;
        int pCnt=0;
        for(int i=0; i<evt.pcnt-1; i++)
        {
            fStatement.add(String(evt.p[i]));
            //cUtils::debug(i, fStatement[i]);
            pCnt=i;
        }

        if(table->genRoutine!=2)
        {
            fStatement.add(String(1));
            fStatement.add(String(evt.p[pCnt]));
        }

        //now set table number and set score char to f
        fStatement.set(1, String(table->tableNumber));
        fStatement.set(0, "f");

        processor.getCsound()->GetCsound()->hfgens(processor.getCsound()->GetCsound(), &ftpp, &evt, 1);
        Array<float, CriticalSection> points;

        points = Array<float, CriticalSection>(ftpp->ftable, ftpp->flen);
        table->setWaveform(points, false);
        //table->enableEditMode(fStatement);

        processor.getCsound()->InputMessage(fStatement.joinIntoString(" ").toUTF8());
    }

}

//======================================================================================================
void CabbagePluginEditor::sendChannelDataToCsound(String channel, float value)
{
    processor.getCsound()->SetChannel(channel.getCharPointer(), value);
}

void CabbagePluginEditor::sendChannelStringDataToCsound(String channel, String value)
{
    processor.getCsound()->SetChannel(channel.getCharPointer(), value.toUTF8().getAddress());
}

const Array<float, CriticalSection> CabbagePluginEditor::getArrayForSignalDisplay(const String signalVariable, const String displayType)
{
    return processor.getSignalArray(signalVariable, displayType)->getPoints();
}

bool CabbagePluginEditor::shouldUpdateSignalDisplay()
{
    return processor.shouldUpdateSignalDisplay();
}

void CabbagePluginEditor::enableXYAutomator(String name, bool enable, Line<float> dragLine)
{
    processor.enableXYAutomator(name, enable, dragLine);
}

bool CabbagePluginEditor::csdCompiledWithoutError()
{
    return processor.csdCompiledWithoutError();
}

StringArray CabbagePluginEditor::getTableStatement(int tableNumber)
{
    return processor.getTableStatement(tableNumber);
}

const Array<float, CriticalSection> CabbagePluginEditor::getTableFloats(int tableNumber)
{
    return processor.getTableFloats(tableNumber);
}

CabbagePluginProcessor& CabbagePluginEditor::getProcessor()
{
    return processor;
}
//======================================================================================================
const String CabbagePluginEditor::getCsoundOutputFromProcessor()
{
#ifndef Cabbage_IDE_Build
    return processor.getCsoundOutput();
#endif
	return String("You are currently in 'Standalone' mode.\nThe csoundoutput widget will only be filled\nwith Csound messages when used in a plugin.");
}