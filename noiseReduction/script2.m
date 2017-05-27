clear;
f = im2single(imread('images/Fig0526a.tif','tif'));
F = fft2(f);
F = fftshift(F);
[G,H] = addMotionBlur(F,1,0.1,0.1);
% not real part too big error
g = ifft2(ifftshift(G));
%gn = addSinNoise(g,10^-2,250,100);
%gn = imnoise(g,'gaussian',0,10^-2);
gn = g;
Gn = fft2(gn);
Gn = fftshift(Gn);

G1 = wienerFiltering(Gn,H,10^-10);
g1 = real(ifft2(ifftshift(G1)));
psnr1 = computePSNR(g1,f);

G2 = wienerFiltering(Gn,H,10^-23);
g2 = real(ifft2(ifftshift(G2)));
psnr2 = computePSNR(g2,f);

G3 = wienerFiltering(Gn,H,0);
g3 = real(ifft2(ifftshift(G3)));
psnr3 = computePSNR(g3,f);

imwrite(im2uint8(real(g)),'images/Fig0526-2b.tif','tif');
imwrite(im2uint8(real(gn)),'images/Fig0526-2bn.tif','tif');
imwrite(im2uint8(g1),'images/Fig0526-1.tif','tif');
imwrite(im2uint8(g2),'images/Fig0526-2.tif','tif');
imwrite(im2uint8(g3),'images/Fig0526-3.tif','tif');
imwrite(im2uint8(rescale(log(1+abs(Gn)))),'images/gaussian.tif','tif');