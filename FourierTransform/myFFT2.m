function [ F ] = myFFT2( f )
%MYFFT2 Summary of this function goes here
%   Detailed explanation goes here
%   each column of FFT
    g = single(ones(size(f)));
    F = single(ones(size(f)));
    
    N = size(f,1);
    s1 = log2(N);
    if (s1 ~= nextpow2(N))
        error('The rows of image must be the pow of 2');
    end
    
    M = size(f,2);
    s2 = log2(M);
    if (s2 ~= nextpow2(M))
        error('The colmuns of image must be the pow of 2');
    end
    K = 0:max(M,N)/2-1;
    % K from 0 ~ N/2-1, but the index is from 1 ~ N/2
    W = exp((-1j*2*pi/N)*K);
    for u = 1 : size(f,1)
        g(u,:) = myFFT(f(u,:),W);
    end
    for v = 1 : size(f,2)
        F(:,v) = myFFT(g(:,v),W);
    end
end
