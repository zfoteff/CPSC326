
%----------------------------------------------------------------------
% Name: Zac Foteff
% File: hw10c.pl
% Date: Spring 2021
% Desc: List method implementations in Prolog
%----------------------------------------------------------------------

% To run tests from command line use:
%   swipl -g run_tests -t halt hw10c.pl
%
% Alternatively, do:
%   swipl hw10c.pl
%   ?- run_tests.
%
% Note that after running swipl hw10c.pl from the command line, your
% source file is loaded into the interpreter and you can do basic
% tests there for debugging.

% Question 1: my_last
my_last([], _).
my_last([_|T], X) :- my_last(T, X).

% Question 2: my_init
my_init([], _).
my_init([_],[]).
my_init([H|T], [H|Ty]) :- my_init(T, Ty).

% Question 3: my_replace
my_replace((_,_), [], []).
my_replace((A,B), [H|T], [B|Ty]) :- H == A, my_replace((A,B),T,Ty).
my_replace((A,B), [H|T], [H|Ty]) :- my_replace((A,B),T,Ty).

% Question 4: my_elem_sum
my_elem_sum(_,[],0).
my_elem_sum(X,[H|T],S) :- H==X, my_elem_sum(X, T, S2), S is S2+X.
my_elem_sum(X,[_|T],S) :- my_elem_sum(X, T, S).


% Question 5: my_min
my_min([H|T], M) :- min_rec([H|T], H, M2), M=M2.
min_rec([],_,_).
min_rec([H|T],M,_) :- H<M, min_rec(T,H,H).
min_rec([_|T],M,_) :- min_rec(T, M, M).


%----------------------------------------------------------------------

:- begin_tests(all_tests).

% Question 1 Tests
test(my_last_1) :- my_last([1],1), !.
test(my_last_2) :- my_last([1,2],2), !.
test(my_last_3) :- my_last([1,2,3],3), !.
test(my_last_4) :- my_last([],_), !.

% Question 2 Tests


% Question 3 Tests


% Question 4 Tests


% Question 5 Tests


:- end_tests(all_tests).
