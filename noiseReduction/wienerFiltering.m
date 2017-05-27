function [ output_f ] = wienerFiltering( input_f,H,K )
%wienerFiltering: filtering by winner filter
%   input_f & output_f 2-D image in frequency domain(centered), type single
%   H : motion blur degradation function in frequency domain, type single
%   K : winner filter parameter
%   W : winner filter
    H_a = (H).^2;
    W = (1./H).*(H_a./(H_a+K));
    output_f = single(W.*input_f);
end

