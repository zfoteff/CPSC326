
%------------------------------------------------------
% Name: Zac Foteff
% File: hw10a.pl
% Date: Spring 2021
% Desc: Basic queries for movies.pl
%------------------------------------------------------

:- consult('movies.pl').

% Question 1: Find all movies that were released in 2006
q1(M) :- movie(M, 2006).

% Question 2: Find movies made in the 1980's
q2(M) :-
  movie(M,Y), (Y < 1990), (Y >= 1980).

% Question 3: Find directors of movies released in 1998
q3(D) :-
  director(M, D), movie(M, Y), (Y == 1998).

% Question 4: Find actors in movies they also directed
q4(A, M) :-
  actor(M, A, _), director(M, D), (D == A).

% Question 5: Find movies in which Frances McDormand and Holly Hunter were co-stars
q5(M) :-
  actress(M, A1, _), actress(M, A2, _),
  (A1 == frances_mcdormand), (A2 == holly_hunter).

% Question 6: Find actor or actress in movie M
star(M, A) :-
  actor(M, A, _) | actress(M, A, _).

% Question 7: Find 2 different stars in the same movie
co_star(A1, A2) :-
  star(M, A1),
  star(M, A2),
  (A1 \== A2).

% Question 8: Find actresses and actors that starred in 2 different movies in the same year
starred_2(A, Y) :-
  actor(M0,A,_),
  actor(M1,A,_),
  (M0\==M1),
  movie(M0,Y),
  movie(M1,Y).
