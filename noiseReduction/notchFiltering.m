function [ output_f, Notch] = notchFiltering( input_f, D0, u0, v0)
%NOTCHFILTERING filtering the input image in notch filter
%   intput_f & output_f : 2-D image in frequency domain(centered)
%            , type single range 0~1
%   Notch: notch reject filter in frequency domain(two-circle version)
%   D0, u0, v0: H(u,v) = 0 if D1(u,v) <= D0 or D2(u,v) <= D0
%       H(u,v) = 1 otherwise
%   D1(u,v) = sqrt((u-M/2-u0)^2 + (v-N/2-v0)^2)
%   D2(u,v) = sqrt((u-M/2+u0)^2 + (v-N/2+v0)^2)
%   M : width of image  (cols y) 
%   N : height of image (rows x)
    M = size(input_f,1);
    N = size(input_f,2);
    u = 0:M-1;
    v = 0:N-1;
    [U,V] = meshgrid(u,v);
    % symmetric in frequency domain
    % U-M/2,V-N/2 to move DC to orgin point
    D1 = (sqrt((U-M/2-u0).^2 + (V-N/2-v0).^2));
    D2 = (sqrt((U-M/2+u0).^2 + (V-N/2+v0).^2));
    Notch = and(D1 > D0, D2 > D0);
    output_f = single(input_f .* Notch);
end

