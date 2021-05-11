
%----------------------------------------------------------------------
% Name: Zac Foteff
% File: hw10b.pl
% Date: Spring 2021
% Desc: Basic function implementations using prolog
%----------------------------------------------------------------------
%
% To run tests from command line use:
%   swipl -g run_tests -t halt hw10b.pl
%
% Alternatively, do:
%   swipl hw10b.pl
%   ?- run_tests.
%
% Note that after running swipl hw10b.pl from the command line, your
% source file is loaded into the interpreter and you can do basic
% tests there for debugging.
%


% Question 1: my_min
my_min(X,Y,X) :- X =< Y.
my_min(X,Y,Y) :- X > Y.

% Question 2: my_median
my_circle_area(R, A) :- A is (pi * (R^^2)).

% Question 3: my_triangle_area
my_triangle_area(B, H, A) :- A is ((B * H) / 2).

% Question 4: my_circle_area
my_circle_area(R, A) :- A is (3.14*(R**2)).

% Question 5: my_midpoint
my_midpoint((X1,Y1), (X2,Y2), (X3, Y3)) :-
  X3 is (X2+X1)/2.0,
  Y3 is (Y2+Y1)/2.0.

% Question 6: my_manhattan_distance
my_manhattan_distance((X1,Y1), (X2,Y2), D) :-
  D is abs(X1-X2)+abs(Y1-Y2).

% Question 7: my_euclidean_distance
my_euclidean_distance((X1,Y1), (X2,Y2), D) :-
  D is sqrt(((X2-X1)**2) + ((Y2-Y1)**2)).

% Question 8: my_range_sum
my_range_sum(S, E, 0) :- S > E.
my_range_sum(S, E, Sum).
my_range_sum(S, E, Sum) :- S < E, N is S+1, my_range_sum(N, E, X), Sum is X+S.

% Question 9: my_gdc
my_gcd(X,Y,D) :- Y==0, D=X.
my_gcd(X,Y,D) :- X==0, D=Y.
my_gcd(X,Y,D) :- X=Y, D=X.
my_gcd(X,Y,D) :- X<Y, Z is Y-X, my_gcd(X,Z,D).
my_gcd(X,Y,D) :- X>Y, my_gcd(Y,X,D).


% Question 10: my_even and my_odd
my_even(X) :- 0 is mod(X, 2).
my_odd(X) :- 1 is mod(X, 2).


%----------------------------------------------------------------------
% UNIT TESTS
%----------------------------------------------------------------------

:- begin_tests(all_tests).

% Question 1 Tests
test(my_min_1) :- my_min(1,2,1), !.
test(my_min_2) :- my_min(2,1,1), !.
test(my_min_3) :- my_min(1,1,1), !.

% Question 2 Tests


% Question 3 Tests
test(my_triangle_area_1) :- my_triangle_area(1,2,1), !.
test(my_triangle_area_2) :- my_triangle_area(3,4,6), !.
test(my_triangle_area_3) :- my_triangle_area(10,10,50), !.

% Question 4 Tests
test(my_circle_area_1) :- my_circle_area(1, 3.14), !.
test(my_circle_area_2) :- my_circle_area(3.5, 38.465), !.
test(my_circle_area_3) :- my_circle_area(150, 70650.0), !.

% Question 5 Tests
test(my_midpoint_1) :- my_midpoint((1,1),(1,1),(1.0,1.0)), !.
test(my_midpoint_2) :- my_midpoint((1,1),(3,5),(2.0,3.0)), !.
test(my_midpoint_3) :- my_midpoint((2.5,1.75),(3,8.75),(2.75,5.25)), !.

% Question 6 Tests
test(my_manhattan_distance_1) :- my_manhattan_distance((1,1),(1,1),0), !.
test(my_manhattan_distance_2) :- my_manhattan_distance((0,-1),(1,1),3), !.
test(my_manhattan_distance_3) :- my_manhattan_distance((-3,1),(1,-4),9), !.

% Question 7 Tests
test(my_euclidean_distance_1) :- my_euclidean_distance((0,0),(0,0), 0.0), !.
test(my_euclidean_distance_2) :- my_euclidean_distance((1,1),(1,1), 0.0), !.
test(my_euclidean_distance_3) :- my_euclidean_distance((1,1),(4,5), 5.0), !.

% Question 8 Tests
test(my_range_sum_1) :- my_range_sum(1, 4, 10), !.
test(my_range_sum_2) :- my_range_sum(2, 4, 9), !.
test(my_range_sum_3) :- my_range_sum(1, 10, 55), !.

% Question 9 Tests
test(my_gcd_1) :- my_gcd(2, 17, 1), !.
test(my_gcd_2) :- my_gcd(2, 18, 2), !.
test(my_gcd_3) :- my_gcd(15, 25, 5), !.

% Question 10 Tests
test(my_even_1) :- my_even(2), !.
test(my_even_2) :- my_even(3), !.
test(my_odd_1) :- my_odd(3), !.
test(my_odd_2) :- my_odd(2), !.


:- end_tests(all_tests).
