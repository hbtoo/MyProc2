function x = calc_KM(R0, P0, PA, f, Pv, n, rho, sigma, miu, c)

%clear;
%clc;
options=odeset('RelTol',1e-9);
%hold on;box on;%生成空的坐标轴
%R0=137.6*10^(-9);%初始半径  ---已确认
%R0=10*10^(-6);%初始半径  ---已确认

%P0=1.01325*10^5;%水中静压   ---已确认
%PA=-7.041*P0;%超声波声压幅值        ---大探头系数小探头用7.041，小探头系数用5.554
%f=20000;%超声频率---已确认
y0=[R0,0,0];%设置初值
[x,y] = ode45('KM',[0,2.5/f],y0,options, R0, P0, PA, f, Pv, n, rho, sigma, miu, c);
R=y(:,1)/R0;%归一化半径
U=y(:,2);

%{
figure
plot(x*1000000,y(:,1)/R0)
title('evolution of bubble radius');
xlabel('time/micron');
ylabel('归一化半径');
figure
plot(R,U)
title('气泡振动相轨迹');
xlabel('气泡半径');
ylabel('泡壁速度');
%}

xlswrite('R_t.xlsx',R);
xlswrite('t.xlsx',x);

x = [1, 2, 3, R0, PA];



