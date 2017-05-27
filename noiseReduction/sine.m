Amp = 1;
freqHz = 12000;
fsHz = 65536;
dt = 1/fsHz;
index = 1;
sines = [];
sampleNumber = [];
for t = 0:dt:1-dt
   sines(index) = Amp * sin(2*pi*freqHz*t);
   sampleNumber(index) = index;
   index = index + 1;
end

% alternative to the above loop
% sine = Amp*sin(2*pi*freq*(0:dt:1-dt));

N = 65536;
transform = fft(sines,N)/N;
magTransform = abs(transform);

faxis = linspace(-fsHz/2,fsHz/2,N);
plot(faxis/1000,fftshift(magTransform));
axis([-40 40 0 0.6])
xlabel('Frequency (KHz)')