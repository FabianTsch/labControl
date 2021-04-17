#include "encoder.h"


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

    emit resultReady(resultAgl);

}
