/*
	 Copyright (C) 2011 Georgia Institute of Technology

	 This program is free software: you can redistribute it and/or modify
	 it under the terms of the GNU General Public License as published by
	 the Free Software Foundation, either version 3 of the License, or
	 (at your option) any later version.

	 This program is distributed in the hope that it will be useful,
	 but WITHOUT ANY WARRANTY; without even the implied warranty of
	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	 GNU General Public License for more details.

	 You should have received a copy of the GNU General Public License
	 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */

#include <math.h>
#include "signal-generator.h"

std::unique_ptr<Modules::Plugin> createRTXIPlugin(Event::Manager* ev_manager)
{
  return std::make_unique<SigGen::Plugin>(ev_manager);
}

Modules::Panel* createRTXIPanel(QMainWindow* main_window,
                                Event::Manager* ev_manager)
{
  return new SigGen::Panel(main_window, ev_manager);
}

std::unique_ptr<Modules::Component> createRTXIComponent(
    Modules::Plugin* host_plugin)
{
  return std::make_unique<SigGen::Component>(host_plugin);
}

Modules::FactoryMethods fact;

extern "C"
{
Modules::FactoryMethods* getFactories()
{
  fact.createPanel = &createRTXIPanel;
  fact.createComponent = &createRTXIComponent;
  fact.createPlugin = &createRTXIPlugin;
  return &fact;
}
};

SigGen::Panel::Panel(QMainWindow* main_window, Event::Manager* ev_manager) :
	Modules::Panel(std::string(SigGen::MODULE_NAME), main_window, ev_manager)
{
	setWhatsThis(
			"<p><b>Signal Generator:</b></p><p>Generates noise of the type specified with the relevant parameters:<br><br>"
			"Sine Wave: frequency, amplitude<br>"
			"Monophasic Square Wave: delay, width, amplitude<br>"
			"Biphasic Square Wave: delay, width, amplitude<br>"
			"Sawtooth Wave: delay, triangle width, triangle peak amplitude<br>"
			"ZAP stimulus: initial frequency, maximum frequency, duration of ZAP<br><br>"
			"The ZAP stimulus has the duration specified. All other signals are continuous signals.</p>");
	initParameters();
	initStimulus();
	this->createGUI(SigGen::get_default_vars(), main_window); // this is required to create the GUI
	customizeGUI();
	update(Modules::Variable::INIT);
	refresh();
	//QTimer::singleShot(0, this, SLOT(resizeMe()));
}

SigGen::Component::Component(Modules::Plugin* hplugin)
	: Modules::Component(hplugin,
			     std::string(SigGen::MODULE_NAME),
			     SigGen::get_default_channels(),
			     SigGen::get_default_vars())
{
}

void SigGen::Component::execute()
{
	switch (this->mode) {
		case SINE:
			writeoutput(0, {sineWave.get()});
			break;

		case MONOSQUARE:
			writeoutput(0, {monoWave.get()});
			break;

		case BISQUARE: 
			writeoutput(0, {biWave.get()});
			break;

		case SAWTOOTH: 
			writeoutput(0, {sawWave.get()});
			break;

		case ZAP:
			writeoutput(0, {zapWave.getOne()});
			break;

		default:
			writeoutput(0, {0});
			break;
	}
}

void SigGen::Panel::update(Modules::Variable::state_t flag)
{
	Modules::Plugin* hplugin = this->getHostPlugin();
	switch (flag) {
		case INIT:
			hplugin->setComponentParameter<double>(SigGen::PARAMETER::FREQ, 
							       QString::number(freq));
			setParameter("ZAP max Freq (Hz)", QString::number(freq2));
			setParameter("ZAP duration (s)", QString::number(ZAPduration));
			setParameter("Delay (s)", QString::number(delay));
			setParameter("Width (s)", QString::number(width));
			setParameter("Amplitude (V)", QString::number(amp));
			waveShape->setCurrentIndex(0);
			updateMode(0);
			break;

		case MODIFY:
			delay = getParameter("Delay (s)").toDouble();
			freq = getParameter("Freq (Hz)").toDouble();
			freq2 = getParameter("ZAP max Freq (Hz)").toDouble();
			ZAPduration = getParameter("ZAP duration (s)").toDouble();
			width = getParameter("Width (s)").toDouble();
			amp = getParameter("Amplitude (V)").toDouble();
			mode = mode_t(waveShape->currentIndex());
			initStimulus();
			break;

		case PERIOD:
			dt = RT::System::getInstance()->getPeriod() * 1e-9; // time in seconds
			initStimulus();

		case PAUSE:
			output(0) = 0.0;
			zapWave.setIndex(0);
			initStimulus();
			break;

		case UNPAUSE:
			break;

		default:
			break;
	}
}

void SigGen::initParameters()
{
	freq = 1; // Hz
	amp = 1;
	width = 1; // s
	delay = 1; // s
	mode = SINE;
	freq2 = 20; // Hz
	ZAPduration = 10; //s

	dt = RT::System::getInstance()->getPeriod() * 1e-9; // s
	output(0) = 0;
}

void SigGen::initStimulus()
{
	switch (mode) {
		case SINE:
			sineWave.clear();
			sineWave.init(freq, amp, dt);
			break;

		case MONOSQUARE: 
			monoWave.clear();
			monoWave.init(delay, width, amp, dt);
			break;

		case BISQUARE: 
			biWave.clear();
			biWave.init(delay, width, amp, dt);
			break;

		case SAWTOOTH:
			sawWave.clear();
			sawWave.init(delay, width, amp, dt);
			break;

		case ZAP: 
			zapWave.clear();
			zapWave.init(freq, freq2, amp, ZAPduration, dt);
			break;

		default:
			break;
	}
}

void SigGen::updateMode(int index)
{
	switch (index) {
		case 0:
			mode = SINE;
			break;
		case 1:
			mode = MONOSQUARE;
			break;
		case 2:
			mode = BISQUARE;
			break;
		case 3:
			mode = SAWTOOTH;
			break;
		case 4:
			mode = ZAP;
			break;
	}
	update(MODIFY);
}

void SigGen::customizeGUI(void)
{
	QGridLayout *customlayout = DefaultGUIModel::getLayout();

	QGroupBox *modeBox = new QGroupBox("Signal Type");
	QVBoxLayout *modeBoxLayout = new QVBoxLayout(modeBox);
	waveShape = new QComboBox;
	modeBoxLayout->addWidget(waveShape);
	waveShape->insertItem(0, "Sine Wave");
	waveShape->insertItem(1, "Monophasic Sqare Wave");
	waveShape->insertItem(2, "Biphasic Square Wave");
	waveShape->insertItem(3, "Sawtooth Wave");
	waveShape->insertItem(4, "Zap Stimulus");
	QObject::connect(waveShape,SIGNAL(activated(int)), this, SLOT(updateMode(int)));
	customlayout->addWidget(modeBox, 0, 0);
	setLayout(customlayout);
}
