function [ psnr ] = computePSNR( input1_s,input2_s )
%COMPUTEPSNR compute psnr
%   psnr(dB): Peak Signal-to-Noise Ratio between original and restored image
%   psnr between input1_s and input2_s
%   peak in gray picture is 255
%   higher psnr means higer quality
    if size(input1_s) ~= size(input2_s)
        error('two input must be equal size');
    end
    M = size(input1_s,1);
    N = size(input1_s,2);
    square_err = (input1_s-input2_s).^2;
    psnr = 10*log10((M*N*255*255)/sum(square_err(:)));
end

