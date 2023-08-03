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

SigGen::Plugin::Plugin(Event::Manager* ev_manager)
    : Modules::Plugin(ev_manager, std::string(SigGen::MODULE_NAME))
{
}

SigGen::Panel::Panel(QMainWindow* main_window, Event::Manager* ev_manager)
    : Modules::Panel(std::string(SigGen::MODULE_NAME), main_window, ev_manager)
{
  setWhatsThis(
      "<p><b>Signal Generator:</b></p><p>Generates noise of the type specified "
      "with the relevant parameters:<br><br>"
      "Sine Wave: frequency, amplitude<br>"
      "Monophasic Square Wave: delay, width, amplitude<br>"
      "Biphasic Square Wave: delay, width, amplitude<br>"
      "Sawtooth Wave: delay, triangle width, triangle peak amplitude<br>"
      "ZAP stimulus: initial frequency, maximum frequency, duration of "
      "ZAP<br><br>"
      "The ZAP stimulus has the duration specified. All other signals are "
      "continuous signals.</p>");
  createGUI(SigGen::get_default_vars(),
            {SIGNAL_WAVEFORM, STATE});  // this is required to create the GUI
  customizeGUI();
  //update(Modules::Variable::INIT);
  //refresh();
}

SigGen::Component::Component(Modules::Plugin* hplugin)
    : Modules::Component(hplugin,
                         std::string(SigGen::MODULE_NAME),
                         SigGen::get_default_channels(),
                         SigGen::get_default_vars())
    , dt(static_cast<double>(RT::OS::getPeriod()) * 1e-9)
{
}

void SigGen::Component::execute()
{
  switch (this->getValue<Modules::Variable::state_t>(SigGen::PARAMETER::STATE)) {
    case Modules::Variable::EXEC:
      writeoutput(0, this->current_generator->get());
      break;
    case Modules::Variable::INIT:
      this->initStimulus();
      this->setValue(SigGen::PARAMETER::STATE, Modules::Variable::EXEC);
      break;
    case Modules::Variable::MODIFY:
      initStimulus();
      this->setValue(SigGen::PARAMETER::STATE, Modules::Variable::EXEC);
      break;
    case Modules::Variable::PERIOD:
      this->dt = static_cast<double>(RT::OS::getPeriod()) * 1e-9;  // time in seconds
      initStimulus();
      this->setValue(SigGen::PARAMETER::STATE, Modules::Variable::EXEC);
      break;
    case Modules::Variable::PAUSE:
      writeoutput(0, 0);
      break;
    case Modules::Variable::UNPAUSE:
      zapWave.setIndex(0);
      this->setValue(SigGen::PARAMETER::STATE, Modules::Variable::EXEC);
      break;
    default:
      break;
  }
}

void SigGen::Panel::update(Modules::Variable::state_t flag)
{
  this->getHostPlugin()->setComponentParameter(SigGen::PARAMETER::STATE, flag);
}

void SigGen::Component::initParameters()
{
  this->setValue<double>(SigGen::PARAMETER::FREQ, 1.0); // Hz
  this->setValue<double>(SigGen::PARAMETER::AMPLITUDE, 1.0);
  this->setValue<double>(SigGen::PARAMETER::WIDTH, 1.0); // s
  this->setValue<double>(SigGen::PARAMETER::DELAY, 1.0); // s
  this->setValue<int64_t>(SigGen::PARAMETER::SIGNAL_WAVEFORM, SigGen::WAVEMODE::SINE);
  this->setValue<double>(SigGen::PARAMETER::ZAP_MAX_FREQ, 20.0); // Hz
  this->setValue<double>(SigGen::PARAMETER::ZAP_DURATION, 10.0); //s
  this->dt = static_cast<double>(RT::OS::getPeriod()) * 1e-9; // s
  this->initStimulus();
}

void SigGen::Component::initStimulus()
{
  auto freq = getValue<double>(SigGen::PARAMETER::FREQ);
  auto max_zap_freq = getValue<double>(SigGen::PARAMETER::ZAP_MAX_FREQ);
  auto delay = getValue<double>(SigGen::PARAMETER::DELAY);
  auto width = getValue<double>(SigGen::PARAMETER::WIDTH);
  auto amp = getValue<double>(SigGen::PARAMETER::AMPLITUDE);
  auto zap_duration = getValue<double>(SigGen::PARAMETER::ZAP_DURATION);
  this->dt = static_cast<double>(RT::OS::getPeriod()) * 1e-9; // s
  switch (this->getValue<int64_t>(SigGen::PARAMETER::SIGNAL_WAVEFORM)) {
    case SigGen::WAVEMODE::SINE:
      this->sineWave.clear();
      this->sineWave.init(freq, amp, this->dt);
      this->current_generator = &this->sineWave;
      break;
    case SigGen::WAVEMODE::MONOSQUARE:
      this->monoWave.clear();
      this->monoWave.init(delay, width, amp, this->dt);
      this->current_generator = &this->monoWave;
      break;
    case SigGen::WAVEMODE::BISQUARE:
      biWave.clear();
      biWave.init(delay, width, amp, this->dt);
      this->current_generator = &this->biWave;
      break;
    case SigGen::WAVEMODE::SAWTOOTH:
      sawWave.clear();
      sawWave.init(delay, width, amp, this->dt);
      this->current_generator = &this->sawWave;
      break;
    case SigGen::WAVEMODE::ZAP:
      zapWave.clear();
      zapWave.init(freq, max_zap_freq, amp, zap_duration, this->dt);
      this->current_generator = &this->zapWave;
      break;
    default:
      break;
  }
}

void SigGen::Panel::updateMode(int index)
{
  auto wave_type = static_cast<SigGen::WAVEMODE::mode_t>(index);
  this->getHostPlugin()->setComponentParameter(SigGen::PARAMETER::SIGNAL_WAVEFORM, wave_type);
  this->getHostPlugin()->setComponentParameter(SigGen::PARAMETER::STATE, Modules::Variable::MODIFY); 
}

void SigGen::Panel::refresh()
{
}

void SigGen::Panel::customizeGUI()
{
  // This may throw if we end up changing the default gui
  // layout from horizontal to something else (like grid)
  auto* widget_layout = dynamic_cast<QVBoxLayout*>(this->layout());

  auto* modeBox = new QGroupBox("Signal Type");
  auto* modeBoxLayout = new QVBoxLayout(modeBox);
  waveShape = new QComboBox;
  modeBoxLayout->addWidget(waveShape);
  waveShape->insertItem(0, "Sine Wave");
  waveShape->insertItem(1, "Monophasic Sqare Wave");
  waveShape->insertItem(2, "Biphasic Square Wave");
  waveShape->insertItem(3, "Sawtooth Wave");
  waveShape->insertItem(4, "Zap Stimulus");
  QObject::connect(
      waveShape, SIGNAL(activated(int)), this, SLOT(updateMode(int)));
  widget_layout->insertWidget(0, modeBox);
  setLayout(widget_layout);
  this->getMdiWindow()->setFixedSize(this->minimumSizeHint());
}
