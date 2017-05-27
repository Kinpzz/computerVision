function [ output_s ] = addSinNoise( input_s, A, u0, v0 )
%addSinNoise add sin noise to input image 
%   intut_s & output_s : 2-D image in spatial
%   u0, v0, A : noise = n(x,y) = A*sin(2*pi*(u0*x/M+v0*y/N))
%   A : the magnitude of noise
%   M : width of image  (cols y) 
%   N : height of image (rows x) 
    M = size(input_s,1);
    N = size(input_s,2);
    x = 0:M-1;
    y = 0:N-1;
    [X,Y]=meshgrid(x,y);
    Noise = A*sin(2*pi*((u0/M).*X+(v0/N).*Y));
    
    output_s = single(input_s + Noise);
end

