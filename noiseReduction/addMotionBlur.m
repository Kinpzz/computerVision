function [ output_f, H ] = addMotionBlur( input_f, T, a, b )
%ADDMOTIONBLUR add motion blur
%   input_f & output_f 2-D image in frequency domain(centered), type single
%   H : motion blur degradation function in frequency domain, type single
%   T, a, b : H(u,v) =
%   (T/pi*(u*a+v*b))*sin(pi*(u*a+v*b))*exp(-1j*pi*(u*a+v*b))
    M = size(input_f,1)/2;
    N = size(input_f,2)/2;
    [U,V]=meshgrid(floor(-M+1):1:floor(M),floor(-N+1):1:floor(N));
    UV = pi.*(a.*U+b.*V);
    H = (T./UV).*sin(UV).*exp(-1j.*UV);
    [n,m] = find(isnan(H));
    for i = 1:length(n)
        H(n(i),m(i))=T;
    end
    output_f = single(H.*input_f);
end

