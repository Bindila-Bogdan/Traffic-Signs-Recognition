# Traffic Signs Recognition

### Real-time traffic signs detection and recognition module for Raspberry Pi

This module represents a component of an autonomous 1/10 scale car with which Back On Track team has competed at Bosch Future Mobility Challenge 2020. It was designed to detect four types of miniture road signs positioned on poles.

## Features

- Does not need calibration
- Two image preprocessing pipelines that run in parallel:
  - ROI extraction based on pole
  - Detection based on color segmentation
- Robust to occlusions and innacurate color representation
- Optimized convolutional neural network
- Inferece done directly in C++
- Computes distances to sign

## Diagram

![Diagram of the system](./paper_presentation/system_diagram.png)

## Performance

- The system is running at 6-10 FPS on a single core of a Raspberry Pi 4 while capturing images in real-time.
- It classifies the traffic signs with an accuracy of 95.85%. 

## Appendix

- For more information please refer the detailed presentation and paper that are placed in [this directory](./paper_presentation/).

## License

[GNU GPLv3](https://choosealicense.com/licenses/gpl-3.0/)
