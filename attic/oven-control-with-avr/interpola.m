load data

tiempo = data(:,1)';
temp   = data(:,2)';

tiempo_interp = [tiempo(1):15:tiempo(length(tiempo))];

res = spline (tiempo, temp, tiempo_interp);

splinedata = [tiempo_interp', res'];

save splinedata splinedata
