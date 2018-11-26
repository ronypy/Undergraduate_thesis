clc;
clear all;
close all;

load matlab a

%X=[0, 50];
%Y=[0, 0];
% X=[050, 050, 200, 200, 150];
% Y=[250, 100, 300, 150, 050];
X=a(:,1);
Y=a(:,2);

NodeNumber = length(X);
Z=zeros(NodeNumber,1);
plot(X,Y,'o');
grid on;


for n=1:NodeNumber
    node(n,[1 2 3])=[X(n) Y(n) Z(n)];
end

for n=1:NodeNumber-1
    SDPairs(n,[1 2])=[n NodeNumber];
end

for n=1:NodeNumber-1
    SDPairs(NodeNumber-1+n,[1 2])=[NodeNumber n];
end

%%%%%%%%
lambda=2;

SimulationTime=10000;
%%%%%%%%%%%
TransmissionPower=-6; %dbm
Pt=(10^(TransmissionPower/10))/1000;
NoisePower=1e-13;
SINRThreshold=4.3651;
Attenuation=3;
CSFactor=2.2;
farfield=1;
% CarrierSensingThreshold=NoisePower*(1+SINRThreshold*CSFactor^(-Attenuation));
CarrierSensingThreshold=1.5e-13;

FlowNumber=length(SDPairs(:,1));
link = 0;
for i = 1:length(node(:,1))
%     k=1;
%     for j=1:length(node(:,1))
%        if (j~=i)
            r(i) = sqrt((node(i,1)-node(20,1))*(node(i,1)-node(20,1))+(node(i,2)-node(20,2))*(node(i,2)-node(20,2)));
            Pr(i) = Pt.*(r(i)^-Attenuation);
            
            SNR(i) = Pr(i)/NoisePower;
            if(SNR(i) >= 4.20)
                link = link + 1;
            end
            
%             k=k+1;
%        end
%     end
end

contention_win = 127*ones(1,NodeNumber);
% contention_win(20)=31;

%routing_param = zeros(FlowNumber, 2, NodeNumber);

% routing_param(:,:,1)=[1,2,3,4,5,6,7,8; 2,0,0,0,2,0,0,0]';
% routing_param(:,:,2)=[1,2,3,4,5,6,7,8; 5,5,0,0,1,2,0,0]';
% routing_param(:,:,3)=[1,2,3,4,5,6,7,8; 0,0,4,0,0,0,4,0]';
% routing_param(:,:,4)=[1,2,3,4,5,6,7,8; 0,0,5,5,0,0,3,4]';
% routing_param(:,:,5)=[1,2,3,4,5,6,7,8; 2,2,4,4,2,2,4,4]';


% routing_param(1,:,:)=[0 1 0 0 0; 0 0 0 0 0; 0 0 0 0 0; 0 0 0 0 0; 0 0 0 0 0; 0 0 0 0 0; 0 0 0 0 0; 0 0 0 0 0];
% routing_param(2,:,:)=[0 0 0 0 1; 0 0 0 0 1; 0 0 0 0 0; 0 0 0 0 0; 1 0 0 0 0; 0 0 0 0 0; 0 0 0 0 0; 0 0 0 0 0];
% routing_param(3,:,:)=[0 0 0 0 0; 0 0 0 0 0; 0 0 0 1 0; 0 0 0 0 0; 0 0 0 0 0; 0 0 0 0 0; 0 0 0 0 0; 0 0 0 0 0];
% routing_param(4,:,:)=[0 0 0 0 0; 0 0 0 0 0; 0 0 0 0 1; 0 0 0 0 1; 0 0 0 0 0; 0 0 0 0 0; 0 0 1 0 0; 0 0 0 0 0];
% routing_param(5,:,:)=[0 0 0 0 0; 0 0 0 0 0; 0 0 0 0 0; 0 0 0 0 0; 0 1 0 0 0; 0 1 0 0 0; 0 0 0 1 0; 0 0 0 1 0];


routing_param=zeros(NodeNumber,FlowNumber,NodeNumber);

for (n=1:NodeNumber-1)
routing_param(n,n,NodeNumber)=1;
routing_param(NodeNumber,NodeNumber+n-1,n)=1;
end


fid=fopen('datacpp.dat', 'w');

%%% Node Number
fprintf(fid,'\n%d\t',NodeNumber);

%%% Flow Number
fprintf(fid,'\n%d\t',FlowNumber);

% NodePosition
for n=1:NodeNumber
    fprintf(fid,'\n%d\t',node(n,1));
end
for n=1:NodeNumber
    fprintf(fid,'\n%d\t',node(n,2));
end
for n=1:NodeNumber
   fprintf(fid,'\n%d\t',node(n,3));
end

% SDPAIRS
for f=1:FlowNumber
        fprintf(fid,'\n%d\t',SDPairs(f,1));
end
for f=1:FlowNumber
        fprintf(fid,'\n%d\t',SDPairs(f,2));
end

% Transmission power,CST
fprintf(fid,'\n%f\t',TransmissionPower);
fprintf(fid,'\n%e\t',CarrierSensingThreshold);


%source rate
        fprintf(fid,'\n%f\t',lambda);
% simulation time
   fprintf(fid,'\n%d\t',SimulationTime);

for n=1:NodeNumber
   fprintf(fid,'\n%d\t',contention_win(n));
end

for n1=1:NodeNumber
    for n2=1:FlowNumber
        for n3=1:NodeNumber
            fprintf(fid,'\n%d\t',routing_param(n1,n2,n3));

        end
    end
end

fclose(fid);



