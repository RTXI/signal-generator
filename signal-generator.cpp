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

#include "signal-generator.h"
#include <math.h>
#include <default_gui_model.h>
#include <main_window.h>
#include <QtGui>

extern "C" Plugin::Object *
createRTXIPlugin(void)
{
  return new SigGen();
}

static SigGen::variable_t vars[] =
  {
    { "Signal Waveform", "Noise Waveform", DefaultGUIModel::OUTPUT, },
        { "Delay", "Delay", DefaultGUIModel::PARAMETER
            | DefaultGUIModel::DOUBLE, },
        { "Width", "Width", DefaultGUIModel::PARAMETER
            | DefaultGUIModel::DOUBLE, },
        { "Freq (Hz)", "Freq (Hz), also used as minimum ZAP frequency",
            DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
        { "Amplitude", "Amplitude", DefaultGUIModel::PARAMETER
            | DefaultGUIModel::DOUBLE, },
        { "ZAP max Freq (Hz)", "Maximum ZAP frequency",
            DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
        { "ZAP duration (s)", "ZAP duration (s)", DefaultGUIModel::PARAMETER
            | DefaultGUIModel::DOUBLE, },
        { "Time (s)", "Time (s)", DefaultGUIModel::STATE, }, };

static size_t num_vars = sizeof(vars) / sizeof(DefaultGUIModel::variable_t);

SigGen::SigGen(void) :
  DefaultGUIModel("Signal Generator", ::vars, ::num_vars)
{
  setWhatsThis(
      "<p><b>Signal Generator:</b></p><p>Generates noise of the type specified with the relevant parameters:<br><br>"
        "Sine Wave: frequency, amplitude<br>"
        "Monophasic Square Wave: delay, pulse width, pulse amplitude<br>"
        "Biphasic Square Wave: delay, pulse width, pulse amplitude<br>"
        "Sawtooth Wave: delay, triangle width, triangle peak amplitude<br>"
        "ZAP stimulus: initial frequency, maximum frequency, duration of ZAP<br><br>"
        "The ZAP stimulus has the duration specified. All other signals are continuous signals.</p>");
  printf("Starting SigGen Module:\n");
  initParameters();
  initStimulus();
  DefaultGUIModel::createGUI(vars, num_vars); // this is required to create the GUI
  customizeGUI();
  update(INIT);
  DefaultGUIModel::refresh();
}

SigGen::~SigGen(void)
{
}

void
SigGen::execute(void)
{
  systime = count * dt; // time in seconds

  if (mode == SINE)
    {
      output(0) = sineWave.get();
    }
  else if (mode == MONOSQUARE)
    {
      output(0) = monoWave.get();
    }
  else if (mode == BISQUARE)
    {
      output(0) = biWave.get();
    }
  else if (mode == SAWTOOTH)
    {
      output(0) = sawWave.get();
    }
  else if (mode == ZAP)
    {
      output(0) = zapWave.getOne();
    }
  else
    {
      output(0) = 0;
    }
  count++; // increment time

}

void
SigGen::update(DefaultGUIModel::update_flags_t flag)
{
  switch (flag)
    {
  case INIT:
    setParameter("Freq (Hz)", QString::number(freq));
    setParameter("ZAP max Freq (Hz)", QString::number(freq2));
    setParameter("ZAP duration (s)", QString::number(ZAPduration));
    setParameter("Delay", QString::number(delay));
    setParameter("Width", QString::number(width));
    setParameter("Amplitude", QString::number(amp));
    setState("Time (s)", systime);
    waveShape->setCurrentIndex(0);
    updateMode(0);
    break;
  case MODIFY:
    delay = getParameter("Delay").toDouble();
    freq = getParameter("Freq (Hz)").toDouble();
    freq2 = getParameter("ZAP max Freq (Hz)").toDouble();
    ZAPduration = getParameter("ZAP duration (s)").toDouble();
    width = getParameter("Width").toDouble();
    amp = getParameter("Amplitude").toDouble();
    mode = mode_t(waveShape->currentIndex());
    initStimulus();
    break;
  case PERIOD:
    dt = RT::System::getInstance()->getPeriod() * 1e-9; // time in seconds
    initStimulus();
  case PAUSE:
    output(0) = 0.0;
    zapWave.setIndex(0);
    break;
  case UNPAUSE:
    systime = 0;
    count = 0;
    break;
  default:
    break;
    }
}

void
SigGen::initParameters()
{
  freq = 1; // Hz
  amp = 1;
  width = 1; // s
  delay = 1; // s
  mode = SINE;
  freq2 = 20; // Hz
  ZAPduration = 10; //s

  dt = RT::System::getInstance()->getPeriod() * 1e-9; // s
  systime = 0;
  count = 0;
  output(0) = 0;

}

void
SigGen::initStimulus()
{

  switch (mode)
    {
  case SINE:
    sineWave.clear();
    sineWave.init(freq, amp, dt);
    break;
  case MONOSQUARE: // triangular
    monoWave.clear();
    monoWave.init(delay, width, amp, dt);
    break;
  case BISQUARE: // Hamming
    biWave.clear();
    biWave.init(delay, width, amp, dt);
    break;
  case SAWTOOTH: // Hann
    sawWave.clear();
    sawWave.init(delay, width, amp, dt);
    break;
  case ZAP: // Dolph-Chebyshev
    zapWave.clear();
    zapWave.init(freq, freq2, amp, ZAPduration, dt);
    break;
  default:
    break;
    }

}

void
SigGen::updateMode(int index)
{
  if (index == 0)
    {
      mode = SINE;
      printf("Signal generator now set to sine wave\n");
      update(MODIFY);
    }
  else if (index == 1)
    {
      mode = MONOSQUARE;
      printf("Signal generator now set to monophasic square wave\n");
      update(MODIFY);
    }
  else if (index == 2)
    {
      mode = BISQUARE;
      printf("Signal generator now set to biphasic square wave\n");
      update(MODIFY);
    }
  else if (index == 3)
    {
      mode = SAWTOOTH;
      printf("Signal generator now set to sawtooth wave\n");
      update(MODIFY);
    }
  else if (index == 4)
    {
      mode = ZAP;
      printf("Signal generator now set to a ZAP stimulus\n");
      update(MODIFY);
    }
}

void SigGen::customizeGUI(void) {
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
//	waveShape->setToolTip("Choose a signal to generate");
	QObject::connect(waveShape,SIGNAL(activated(int)), this, SLOT(updateMode(int)));

	customlayout->addWidget(modeBox, 0, 0);
	setLayout(customlayout);
}
