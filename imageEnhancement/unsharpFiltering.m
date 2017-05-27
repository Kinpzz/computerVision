function [ output, scaledUnsharp, blurredInput] = unsharpFiltering( input, boxMask, scale)
% unsharp filtering
%   boxMask box(average) mask
%   blurredInput input after applying box mask type single
%   scaledUnsharp c*g_mask
    blurredInput = spatialFiltering(input,boxMask);
    scaledUnsharp = scale .* (input - blurredInput);
    output = input + scaledUnsharp;
end

