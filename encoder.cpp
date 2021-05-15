#include "encoder.h"

static int transits[16]=
{
/* 0000 0001 0010 0011 0100 0101 0110 0111 */
      0,  -1,   1,   0,   1,   0,   0,  -1,
/* 1000 1001 1010 1011 1100 1101 1110 1111 */
     -1,   0,   0,   1,   0,   1,  -1,   0
};

void _cb(int pi, unsigned gpio, unsigned level, uint32_t tick, void *user)
{

    Encoder *self = (Encoder *)user;
    int newState, inc, detent;

    if(level != PI_TIMEOUT){
        if(gpio == self->gpioA)
            self->levA = level;
        else
            self->levB = level;
        newState = self->levA << 1 | self->levB;
        inc = transits[self->oldState << 2 | newState];

        if(inc){
            self->oldState = newState;

            detent = self->step / 4;

            self->step += inc;

        }
    }
}

void _cb_singel_rise(int pi, unsigned gpio, unsigned level, uint32_t tick, void *user)
{
    Encoder *self = (Encoder *)user;
    if(level != PI_TIMEOUT){
        self->step++;
    }
    qDebug() << "Ich wurde ausgelöst" << self->step;
}

Encoder::Encoder(int Pi, int GpioA, int GpioB)
    : pi(Pi),
      gpioA(GpioA),
      gpioB(GpioB)
{
   levA = 0;
   levB = 0;
   step = 0;

   set_mode(pi, gpioA, PI_INPUT);
   set_mode(pi, gpioB, PI_INPUT);

   // pull up is needed as encoder common is grounded

   set_pull_up_down(pi, gpioA, PI_PUD_UP);
   set_pull_up_down(pi, gpioB, PI_PUD_UP);

   glitch = 1000;

   set_glitch_filter(pi, gpioA, glitch);
   set_glitch_filter(pi, gpioB, glitch);

   oldState = (gpio_read(pi, gpioA) << 1 | gpio_read(pi, gpioB));

   // monitor encoder level changes
   void *self = this;
   CBFuncEx_t fptr = (CBFuncEx_t)_cb;
   //CBFuncEx_t fptr = _cb;
   //cb_id_a = callback_ex(pi, gpioA, EITHER_EDGE, fptr, self);
   //cb_id_b = callback_ex(pi, gpioB, EITHER_EDGE, fptr, self);
   cb_id_a = callback_ex(pi,gpioA, RISING_EDGE,
                         (CBFuncEx_t) _cb_singel_rise, self);
}

Encoder::~Encoder()
{

}

void Encoder::measure(){
    QVector<double> resultAgl;
    QVector<double> resultSpd;
    QVector<double> resultAcc;
    double dblAngle = 0.0;

    for(size_t k = 0; k < 10; ++k)
    {
        // Condition Conditions to determine direction and speed
        // Channel B is leading Channel A
        int chA;
        int chB;
        for(size_t i = 0; i < readingRounds; ++i)
        {
            chA = digitalRead(CHA);
            chB = digitalRead(CHB);

            if(chA_past == chB_past && chA_past != chA)
              dblAngle += ANGLE_STEP;
            else if(chA_past == chB_past && chB_past != chB)
              dblAngle -= ANGLE_STEP;

            if(chA_past != chB_past && chB != chB_past)
              dblAngle += ANGLE_STEP;
            else if(chA_past != chB_past && chA != chA_past)
              dblAngle -= ANGLE_STEP;

            chA_past = chA;
            chB_past = chB;
            QThread::usleep(150);
        }
        resultAgl.push_back(dblAngle);
    }


}

void Encoder::getResults()
{
    emit resultReady(step);
}
