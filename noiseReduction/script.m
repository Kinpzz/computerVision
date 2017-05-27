clear;
f = imread('images/Fig0526a.tif', 'tif');
f = im2single(f); % im2single map into 0~1
g = addSinNoise(f,1,250,100);
%G = getSpectrum(g);
G = fft2(g);
G = fftshift(G);
imshow(rescale(log(1+abs(G))));
F = fft2(f);
F = fftshift(F);
% why is double times?
[G_f, Notch] = notchFiltering(G,10,250,100);
f_s = ifft2(ifftshift(G_f));

imwrite(im2uint8(g),'images/Fig0526b.tif','tif');
imwrite(im2uint8(rescale(log(1+abs(F)))),'images/Fig0526c.tif','tif');
imwrite(im2uint8(rescale(log(1+abs(G)))),'images/Fig0526d.tif','tif');
imwrite(im2uint8(rescale(log(1+abs(G_f)))),'images/Fig0526e.tif','tif');
imwrite(im2uint8(Notch),'images/Fig0526f.tif','tif');
imwrite(im2uint8(f_s),'images/Fig0526g.tif','tif');
psnr = computePSNR(f,f_s)
T_f = getSpectrum(f_s);
%imshow(rescale(log(1+abs(T_f))));
