% File: cross_cor.m
% Author: ryanlei
% Creation: 2009/04/01
% Modification: 2009/04/02
% Description: 
%   2D Cross-Correlation Operations.
%   Filter must be odd*odd square matrix.
%   Boarder pixels are handled by reflection about the borders.

% result: return value list
% cross_cor: function name, also the file name
% image, filter: parameter list
function [ result ] = cross_cor ( image, filter )
    % dimension 1 is # of row, dimension 2 is # of col
    K = size( filter, 1 );
    L = size( filter, 2 );
    M = size( image, 1 );
    N = size( image, 2 );
    % check dimensions: square, odd
    if K ~= L || mod( K, 2 ) == 0
        disp( 'Filter dimension error.' );
        return;
    end

    result = zeros( M, N );
    % ignore the border for now
    for x = 1 : M 
        for y = 1 : N
            for u = 1 : K
                for v = 1 : K
                    % in MATLAB:
                    %           K   K                       K+1           K+1
                    % g(x,y) = sum sum[ h(u,v) * f( x + u - --- , y + v - ---) ]
                    %          u=1 v=1                       2             2
                    
                    % in C:
                    %          K-1 K-1                      K-1           K-1          
                    % g(x,y) = sum sum[ h(u,v) * f( x + u - --- , y + v - ---) ]
                    %          u=0 v=0                       2             2
                    
                    % default targets
                    targetX = x + u - (K+1)/2;
                    targetY = y + v - (K+1)/2;
                    
                    % out-of-boarder pixels in MATLAB (this is more difficult):
                    % reflection about x = 1 and x = M
                    if ( targetX < 1 ) targetX = 2 - targetX; end    % 1 + (1-tX)
                    if ( targetX > M ) targetX = 2*M - targetX; end  % M - (tX-M)
                    % reflection about y = 1 and y = N
                    if ( targetY < 1 ) targetY = 2 - targetY; end    % 1 + (1-tY)
                    if ( targetY > N ) targetY = 2*N - targetY; end  % N - (tY-N)

                    % ... in MATLAB means line is not over
                    result( x, y ) = result( x, y ) + filter( u, v ) * image( targetX, targetY );
                
                end
            end
        end
    end
end