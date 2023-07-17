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
class Panel : public Modules::Panel
{
public:

    Panel();
    virtual ~Panel();

    void execute();
    void customizeGUI();

    enum mode_t {
        SINE=0, MONOSQUARE, BISQUARE, SAWTOOTH, ZAP,
    };

public slots:

signals: // custom signals

protected:

    virtual void update();

private:

    void initParameters();
    void initStimulus(); // creates SigGen stimuli

    double freq;
    double delay;
    double width;
    double amp;
    double freq2;
    double ZAPduration;
    mode_t mode;

    GeneratorSine sineWave;
    GeneratorMono monoWave;
    GeneratorBiphase biWave;
    GeneratorSaw sawWave;
    GeneratorZap zapWave;

    double dt;

    // QT components
    QPushButton *sineButton;
    QComboBox *waveShape;

private slots:

    void updateMode(int);
};

}
