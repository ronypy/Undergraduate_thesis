clc;
clear all;
%close all;

%a = rand(20,2);
%a=a*100;
%a=[a' [50 50]']';
a=[15.3657   34.4462
   28.1005   78.0520
   44.0085   67.5332
   52.7143    0.6715
   87.5372   38.6771
   51.8052   91.5991
   94.3623    0.1151
   63.7709   46.2449
   95.7694   42.4349
   24.0707   46.0916
   67.6122   77.0160
   28.9065   32.2472
   67.1808   78.4739
   69.5140   47.1357
    6.7993    3.5763
   25.4790   17.5874
   22.4040   72.1758
   66.7833   47.3486
   84.4392   15.2721
   45.7424   60.2170];
plot(a(:,1),a(:,2),'o');
grid on;