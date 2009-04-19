%%% Note: to improve, try multiple thresholds
% [a] minimum edge value
% [b] min_vote >= Diagonal * 0.X
% [c] min_vote >= max_vote * 0.X
% [d] choose no more than XX best votes
% And use cells to avoid too many lines
%%%

% File: hough.m
% Author: ryanlei
% Creation: 2009/04/19
% Modification: XXXX/XX/XX
% Description: The Hough transform which starts with nothing.
clear all;
close all;

img = color2gray( imread( '../pics/hough/paint_hough.bmp' ) );  % original image in grayscale
M = size( img, 1 );
N = size( img, 2 );
% voting matrix: -D <= rho <= D (D = diagonal); 1' <= theta < 180'
D = round( sqrt( M^2 + N^2 ) );
vote = zeros( 2 * D + 1, 180 );  

% #define the threshold edge value
min_edge = 30;      
% [a] #define the threshold number of votes
min_vote = D * 0.2;
% or [b] #define the ratio to the maximum votes
vote_ratio = 0.75;

% COS_SIN_COT table
i = [ 1 : 180 ]';
COS = cosd( i );
SIN = sind( i );
COT = cotd( i );



%%% [1] Obtain an edge image
%%% Other filters ??
% use the Laplacian operator
%filter = [0,-1,0; -1,4,-1; 0,-1,0];
% use the Sobel operator
filter = [-1,0,1; -2,0,2; -1,0,1];
% obtain the "edge image"
edge = abs( filter2( filter, img ) );
%%% chop the border pixels [ SHOULD NOT DO THIS IN C => Use reflection ]
edge( 1, : ) = 0;
edge( M, : ) = 0;
edge( :, 1 ) = 0;
edge( :, N ) = 0;

edge = map_0_255( edge );
figure( 1 );
imshow( edge );
title( 'edge image' );

%%% [2] The voting process
% only keep those > edge_min and record their (x,y)
[ x, y ] = find( edge >= min_edge );

for THETA = 1 : 180
    % formula : x*cos(theta) + y*sin(theta) = rho
    % right-shift RHO by (D+1) to avoid negative indices
    RHO = round( x .* COS(THETA) + y .* SIN(THETA) + (D + 1) );
    for i = 1 : size( RHO )
        vote( RHO( i ), THETA ) = vote( RHO( i ), THETA ) + 1;  % vote for this (rho,theta)            
    end
end

% show the voting result
% figure( 2 );
% imshow( map_0_255( vote ) );

%%% [3] Plot the lines based on the voted rho's and theta's
% plot the straight line using rho's and theta's that received top votes
% plot to the matrix "hough"
% slope = -cot( theta )
% y-intercept = rho * csc( theta )
result = edge;
result( :, :, 2 ) = result( :, :, 1 );
result( :, :, 3 ) = result( :, :, 2 );

% record the rho's and theta's
%%% the threshold
[ rho, theta ] = find( vote >= min_vote );
% left-shift rho by (D+1)
rho = rho - ( D + 1 );
slope = -COT( theta );
y_int = rho ./ SIN( theta );

% plot: |slope| <=   1, then plot along x-axis
%   1 < |slope| <= 100, then plot along y-axis
%       |slope| >  100, then plot vertical line

for line = 1 : size( slope, 1 )
    if abs( slope( line ) ) <= 1
        % plot with delta[x] = 1
        for x = 1 : M
            % y = a * x + b
            y = round( slope( line ) * x + y_int( line ) );
            if y > 1 && y <= N
                result( x, y, 1 ) = 255;
                result( x, y, 2 ) = 0;
                result( x, y, 3 ) = 0;
            end
        end
    elseif abs( slope( line ) ) > 100
        % plot vertically
        for y = 1 : N
            x = rho( line );
            if x > 1 && x <= M
                result( x, y, 1 ) = 255;
                result( x, y, 2 ) = 0;
                result( x, y, 3 ) = 0;
            end
        end        
    else
        % plot with delta[y] = 1
        for y = 1 : N
            % x = ( y - b ) / a
            x = round( ( y - y_int( line ) ) / slope( line ) );
            if x > 1 && x <= M
                result( x, y, 1 ) = 255;
                result( x, y, 2 ) = 0;
                result( x, y, 3 ) = 0;
            end
        end
            
    end
end

figure( 3 );
imshow( result );
title( 'Hough Transform' );
