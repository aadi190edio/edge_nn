#ifndef NN_H
#define NN_H
#include <math.h>

#define INPUT 2
#define HIDDEN 16
#define OUTPUT 3
#define EPOCHS 20

#define TEMP_MIN 50.0f
#define TEMP_RANGE 70.0f
#define VIB_MIN 0.3f
#define VIB_RANGE 5.7f

// ReLU activation
float relu(float x){ return (x > 0) ? x : 0; }
float relu_derivative(float x){ return (x > 0) ? 1.0f : 0.0f; }

// Argmax
int argmax(float a[], int n){
    int idx = 0;
    for(int i=1;i<n;i++)
        if(a[i] > a[idx]) idx = i;
    return idx;
}

// Proper softmax with exponential
void softmax(float x[], int n, float out[]){
    float max_x = x[0];
    for(int i=1;i<n;i++) if(x[i] > max_x) max_x = x[i];

    float sum = 0;
    for(int i=0;i<n;i++){
        out[i] = exp(x[i] - max_x);
        sum += out[i];
    }

    for(int i=0;i<n;i++) out[i] /= sum;
}

// Forward pass with proper softmax
void forward(float in[],
             float W1[HIDDEN][INPUT], float b1[HIDDEN],
             float W2[OUTPUT][HIDDEN], float b2[OUTPUT],
             float h[], float o[]){

    // Normalize inputs: temp [50-120] -> [0,1], vibration [0.3-6] -> [0,1]
    float norm_in[INPUT];
    norm_in[0] = (in[0] - TEMP_MIN) / TEMP_RANGE;
    norm_in[1] = (in[1] - VIB_MIN) / VIB_RANGE;

    for(int i=0;i<HIDDEN;i++){
        h[i]=0;
        for(int j=0;j<INPUT;j++)
            h[i]+=W1[i][j]*norm_in[j];
        h[i]=relu(h[i]+b1[i]);
    }

    float logits[OUTPUT];
    for(int i=0;i<OUTPUT;i++){
        logits[i]=0;
        for(int j=0;j<HIDDEN;j++)
            logits[i]+=W2[i][j]*h[j];
        logits[i]+=b2[i];
    }

    softmax(logits, OUTPUT, o);
}

#endif
