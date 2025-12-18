function dy=KM(t,y,flag, R0, P0, PA, f, Pv, n, rho, sigma, miu, c)
dy = zeros(3,1);    % a column vector
%基本参数
%R0=137.6*10^(-9);%超声空化泡初始半径
%P0=1.01325*10^5;%大气压---已确认
%Pv=5522;%饱和蒸汽压---已确认
%n=1.388;%绝热指数---已确认       %water vapor 1.33，oxygen vapor 1.4，惰性气体为1.67，本研究选择1.4
%rho=930.33;%水的密度---已确认
%sigma=57.46*10^(-3);%表面张力系数---已确认
%miu=0.8703*10^(-3);%粘性系数---已确认
%c=1363.8;%水中声速---已确认
%Keller-Miksis方程
dy=[y(2);(-y(2)^2*(3-y(2)/c)/2+(1+(1-3*n)*y(2)/c)*((P0-Pv)/rho+2*sigma/(rho*R0))*(R0/y(1))^(3*n)-2*sigma/(rho*y(1))-4*miu*y(2)/(rho*y(1))-(1+y(2)/c)*(P0-Pv+PA*sin(2*pi*y(3)))/rho-y(1)*2*pi*f*PA*cos(2*pi*y(3))/(rho*c))/((1-y(2)/c)*y(1)+4*miu/(rho*c));f];
