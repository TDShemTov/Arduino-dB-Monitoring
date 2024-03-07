# Arduino Sound Level Monitor
## Project Description

This project uses an Arduino Uno connected to a MAX4466 microphone to read decibel values from its surroundings. It represents these values through a gradient meter and an endlessly shifting graph.

## Key Features

- Analog Input Reading: Using analog input from the MAX4466 microphone, I converted the data into decibels using the voltage-to-decibel formula.
- Gradient Meter: Utilizng 16-bit color masking, I designed gradient meter to showcase decibel values for more aesthetically pleasing visuals and readability.
- Live Line Graph: Designed so each mirophone input, the graph updates itself instantly showing a clear moving connected line graph that shifts itself endlessly.
- Screen Optimization: I made area specific refreshers to deal with clusters and remove factors that can hinder readability from screen. This again, also updates on each new microphone input.


## Bar not full, I sleep 

![i sleep](https://github.com/TDShemTov/Arduino-dB-Monitoring/assets/148292347/d480fb25-f021-48da-8c0f-9045de279836)

## Bar is full, I am awake
  
![fullbar](https://github.com/TDShemTov/Arduino-dB-Monitoring/assets/148292347/fc6b148e-b6ee-4646-afbc-d161ce45101e)


