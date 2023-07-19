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
#include <QComboBox>
#include <rtxi/gen/gen_sine.h>
#include <rtxi/gen/gen_mono.h>
#include <rtxi/gen/gen_biphase.h>
#include <rtxi/gen/gen_saw.h>
#include <rtxi/gen/gen_zap.h>
#include <rtxi/module.hpp>

namespace SigGen {

constexpr std::string_view MODULE_NAME = "Signal Generator";

enum PARAMETER : Modules::Variable::Id
{
    SIGNAL_WAVEFORM = 0,
    DELAY,
    WIDTH,
    FREQ,
    AMPLITUDE,
    ZAP_MAX_FREQ,
    ZAP_DURATION,
    STATE
};

inline std::vector<Modules::Variable::Info> get_default_vars()
{
    return 
    {
	{ 
            PARAMETER::SIGNAL_WAVEFORM,
            "Signal Waveform", 
            "Signal Waveform", 
            Modules::Variable::DOUBLE_PARAMETER, 
            0.0
	},
	{
            PARAMETER::DELAY,
            "Delay (s)", 
            "Delay (s)",
            Modules::Variable::DOUBLE_PARAMETER,
            0.0
	},
	{
            PARAMETER::WIDTH,
	    "Width (s)", 
            "Width (s)", 
            Modules::Variable::DOUBLE_PARAMETER,
            0.0
	},
	{
            PARAMETER::FREQ,
            "Freq (Hz)", 
            "Freq (Hz), also used as minimum ZAP frequency",
	    Modules::Variable::DOUBLE_PARAMETER,
            0.0
	},
	{
            PARAMETER::AMPLITUDE,
            "Amplitude (V)", 
            "Amplitude (V)", 
            Modules::Variable::DOUBLE_PARAMETER,
            0.0
	},
	{
            PARAMETER::ZAP_MAX_FREQ,
	    "ZAP max Freq (Hz)", 
            "Maximum ZAP frequency",
	    Modules::Variable::DOUBLE_PARAMETER,
            0.0
	},
	{
            PARAMETER::ZAP_DURATION,
	    "ZAP duration (s)", 
            "ZAP duration (s)", 
            Modules::Variable::DOUBLE_PARAMETER,
            0.0
	},
        {
            PARAMETER::STATE,
            "Plugin State",
            "The current state of the plugin. Refer to Modules::Variable::state_t",
            Modules::Variable::STATE,
            Modules::Variable::INIT
        }
    };
}

inline std::vector<IO::channel_t> get_default_channels()
{
    return 
    {
        {
            "Signal Generator Output",
            "Signal Generator Output",
            IO::OUTPUT,
            1
        }
    };
}

class Panel : public Modules::Panel
{
    Q_OBJECT
public:
    Panel(QMainWindow* main_window, 
          Event::Manager* ev_manager);

    void customizeGUI();

protected:
    void update(Modules::Variable::state_t flag) override;

private:

    void initParameters();
    void initStimulus(); // creates SigGen stimuli


    // QT components
    QPushButton *sineButton;
    QComboBox *waveShape;

private slots:

    void updateMode(int);
};

class Component : public Modules::Component
{
public:
    explicit Component(Modules::Plugin* hplugin);
    void execute() override;
private:
    enum mode_t {
        SINE=0, MONOSQUARE, BISQUARE, SAWTOOTH, ZAP,
    };

    GeneratorSine sineWave;
    GeneratorMono monoWave;
    GeneratorBiphase biWave;
    GeneratorSaw sawWave;
    GeneratorZap zapWave;

    //double freq;
    //double delay;
    //double width;
    //double amp;
    //double freq2;
    //double ZAPduration;

    double dt;
    mode_t mode;
};

class Plugin : public Modules::Plugin
{
public:
   explicit Plugin(Event::Manager* ev_manager); 
};

} // namespace SigGen
