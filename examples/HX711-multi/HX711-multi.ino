#include "HX711-multi.h"

// Pins to the load cell amp
#define CLK 2      // clock pin to the load cell amp
#define DOUT1 3    // data pin to the first lca
#define DOUT2 4    // data pin to the second lca
#define DOUT3 5    // data pin to the third lca

byte DOUTS[3] = {DOUT1, DOUT2, DOUT3};

#define CHANNEL_COUNT sizeof(DOUTS)/sizeof(byte)

long int results[CHANNEL_COUNT];

HX711MULTI scales(CHANNEL_COUNT, DOUTS, CLK);

void setup() {
  Serial.begin(115200);
  Serial.println("READY.");
}

/**
 * 
 * This test checks how long sampling takes. With an HX711 set to 'high' speed (documented 80Hz) it is possible to sample it at as high as ~90hz
 * 
 * Notice that if you time a single sample, it might take less than 10ms, (seemingly >100Hz) but this might be a result of your timing being 'good', 
 * meaning that you're sampling when a sample is already waiting (HX711 is in 'ready' state) and thus takes less time.
 *
 * The speedTest will show how fast consecutive samples can be acquired. Take into account that you should have all HX711 chips configured similarly, to get the best results.
 * If even one of the chips is configured for 'slow' mode, the sampling rate will be low for all the chips, while the accuracy you'll get from the 'fast' ones will still be compromised.
 * 
 */
void speedTest(int cnt = 100) {
  Serial.print("Running speed test (");
  Serial.print(cnt);
  Serial.println(" samples):");
  Serial.flush();

  unsigned long a = micros();
  for (int i=0; i<cnt; ++i) {
    scales.read();
  }
  unsigned long b = micros();

  float avgTime = (b-a)/(1000.0*cnt);
  Serial.print("Samples took ");
  Serial.print(avgTime,2);
  Serial.print("ms each on average. (");
  Serial.print(1000/avgTime,0);
  Serial.println("hz)");
  Serial.println();
}

/**
 * This test reads and displays the values for all the configured sensors in the HX711-multi instance
 */
void valueTest(int cnt = 10) {
  Serial.print("Getting ");
  Serial.print(cnt);
  Serial.println(" samples:");
  for (int i=0; i<cnt; ++i) {
    scales.read(results);
  
    for (int i=0; i<scales.get_count(); ++i) {
      Serial.print(results[i],DEC);  
      Serial.print("\t");  
    }  
    Serial.println();  
  }
  Serial.println();
}


/**
 * This test displays the spread of values per sensor over 'cnt' consecutive samples, if there is no change in force applied to the sensor, then this is the magnitude of noise.
 */
void spreadTest(int cnt=100) {
  if (cnt<=1) {
    Serial.print("there is no sense in running a spreadTest with less than 2 samples."); //actually, the code will break if you try to run with cnt=1  
    return;
  }
  
  int i,j;
  Serial.print("Running spraed test (");
  Serial.print(cnt);
  Serial.println(" samples):");
  Serial.flush();


  long int maxResults[CHANNEL_COUNT];
  long int minResults[CHANNEL_COUNT];

  //initialize minimums to be maximum, and maximums to be minimum;
  for (i=0; i<CHANNEL_COUNT; ++i) {
    maxResults[i] = 0x80000000;
    minResults[i] = 0x7FFFFFFF;
  }

  for (i=0; i<cnt; ++i) {    
    scales.read(results);// get values

    //set min/max appropriately, if new ones found
    for (j=0; j<CHANNEL_COUNT; ++j) {
      if (results[j] < minResults[j]) {
        minResults[j] = results[j];
      } else if (results[j]>maxResults[j]) {
        maxResults[j] = results[j];
      }
    }
  }

  for (j=0; j<CHANNEL_COUNT; ++j) {
      long spread = maxResults[j]-minResults[j];
      Serial.print(spread,DEC);  

      Serial.print("\t"); 
  }
  Serial.println();
  Serial.println();
}

void loop() {

  //will test the speed of sampling, using an average of 80 samepls
  speedTest(80);

  //will display 30 consecutive samples
  valueTest(30);

  //will display the spread of 200 consecutive samples, per channel (this is the magnitude of noise, if you do not change the force on the sensor during the test)
  spreadTest(200);

  Serial.println("-----------------");
  delay(2000); //slow things down a bit for ease of reading

}
