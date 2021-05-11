(*
  Name: Zac Foteff
  File: hw7.ml
  Date: 4/8/2021
  Desc: Multiple function implementations for hw1
*)

(* ------ Function Definitions ----- *)
(* Question 1: *)
let my_min x y =
  if (<=) x y then x else y ;;
;;

(* Question 2: *)
let my_median x y z =
  if x > y then
    if x < z then x
    else if y > z then y
    else z
  else
    if x > z then x
    else if y < z then y
    else z
;;

(* Question 3: *)
let my_triangle_area b h =
  0.5 *. b *. h ;;
;;

(* Question 4: *)
let my_circle_area radius =
  let pi = 3.14 in pi *. (radius ** 2.)
;;

(* Question 5: *)
let my_midpoint (x1, y1) (x2, y2) =
  let x_avg = (x2 +. x1) /. 2.
  and y_avg = (y2 +. y1) /. 2. in (x_avg, y_avg)
;;

(* Question 6: *)
let my_manhattan_distance (x1,y1) (x2,y2) =
  let x_dist = abs_float(x1 -. x2)
  and y_dist = abs_float(y1 -. y2)
  in x_dist +. y_dist
;;

(* Question 7: *)
let my_euclidean_distance (x1, y1) (x2, y2) =
  let x_comp = ((x2-.x1) ** 2.)
  and y_comp = ((y2-.y1) ** 2.)
  in sqrt(x_comp +. y_comp)
;;

(* Question 8: *)
let rec my_range_sum v1 v2 =
  if v1 > v2 then 0 else
  if v1 == v2 then v2 else v1 + my_range_sum (v1+1) v2
;;

(* Question 9: *)
let rec my_gcd x y =
  if x == 0 then y else if y == 0 then x else
    if x > y then my_gcd ((mod) x y) y else my_gcd x ((mod) y x)
;;

(* Question 10: *)
let rec my_even x =
  if x < 0 then false else if x == 0 then true else my_odd (x-1)
and my_odd x =
  if x < 0 then false else if x == 1 then true else my_even (x-1)
;;

(* ------ Testing ----- *)
let assert_equal v1 v2 msg =
  let cond = v1 = v2 in
  assert (if not cond then print_endline ("TEST FAILED: " ^ msg) else print_endline("TEST PASSED: " ^ msg) ; cond)
;;

(* Question 1: my_min *)
print_endline ("--------------- my_min tests ---------------") ;;
assert_equal 1 (my_min 1 2) "1 = my_min 1 2" ;;
assert_equal 1 (my_min 2 1) "1 = my_min 2 1" ;;
assert_equal 1 (my_min 1 1) "1 = my_min 1 1" ;;
assert_equal 1. (my_min 1. 2.) "1. = my_min 1. 2." ;;
assert_equal 1. (my_min 2. 1.) "1. = my_min 2. 1." ;;
assert_equal 1. (my_min 1. 1.) "1. = my_min 1. 1." ;;
assert_equal 5 (my_min 5 100000000) "5 (my_min 5 100000000)" ;;
assert_equal 1235 (my_min 100000000 1235) "1235 (my_min 100000000 1235)" ;;

(* Question 2: my_median *)
print_endline ("--------------- my_median tests ---------------") ;;
assert_equal 2 (my_median 2 1 2) "2 (my_median 2 1 2)" ;;
assert_equal 2 (my_median 2 2 2) "2 (my_median 2 2 2)" ;;
assert_equal 2 (my_median 3 1 2) "2 (my_median 3 1 2)" ;;
assert_equal 84 (my_median 100 75 84) "84 (my_median 100 75 84)" ;;
assert_equal 2. (my_median 2. 1. 2.) "2. (my_median 2. 1. 2.)" ;;
assert_equal 2. (my_median 2. 2. 2.) "2. (my_median 2. 2. 2.)" ;;
assert_equal 2. (my_median 3. 1. 2.) "2. (my_median 3. 1. 2.)" ;;
assert_equal 2.2 (my_median 2.1 2.2 2.3) "2.2 (my_median 2.1 2.2 2.3)" ;;

(* Question 3: my_triangle_area *)
print_endline ("--------------- my_triangle_area tests ---------------") ;;
assert_equal 1. (my_triangle_area 1. 2.) "1. (my_triangle_area 1. 2.)" ;;
assert_equal 6. (my_triangle_area 3. 4.) "6. (my_triangle_area 3. 4.)" ;;
assert_equal 12.5 (my_triangle_area 5. 5.) "12.5 (my_triangle_area 5. 5.)" ;;
assert_equal 18. (my_triangle_area 6. 6.) "18. (my_triangle_area 6. 6.)" ;;

(* Question 4: my_circle_area *)
print_endline ("--------------- my_circle_area tests ---------------") ;;
assert_equal 3.14 (my_circle_area 1.) "3.14 (my_circle_area 1.)" ;;
assert_equal 38.465 (my_circle_area 3.5) "38.465 (my_circle_area 3.5)" ;;
assert_equal 70650. (my_circle_area 150.) "70650 (my_circle_area 150.)" ;;

(* Question 5: my_midpoint *)
print_endline ("--------------- my_midpoint tests ---------------") ;;
assert_equal (1.,1.) (my_midpoint (1.,1.) (1.,1.)) "(1.,1.) (my_midpoint (1.,1.) (1.,1.))" ;;
assert_equal (2.,3.) (my_midpoint (1.,1.) (3.,5.)) "(2.,3.) (my_midpoint (1.,1.) (3.,5.))" ;;
assert_equal (2.5, 10.) (my_midpoint (2., 12.) (3., 8.)) "(2.5, 5.) (my_midpoint (2., 12.) (3., 8.))" ;;
assert_equal (2.75, 5.25) (my_midpoint (2.5, 1.75) (3., 8.75)) "(2.75, 5.25) (my_midpoint (2.5, 1.75) (3., 8.75))" ;;
;;

(* Question 6: my_manhattan_distance *)
print_endline ("--------------- my_manhattan_distance tests ---------------") ;;
assert_equal 0. (my_manhattan_distance (0., 0.) (0., 0.)) "0. (my_manhattan_distance (0., 0.) (0., 0.))" ;;
assert_equal 0. (my_manhattan_distance (1., 1.) (1., 1.)) "0. (my_manhattan_distance (1., 1.) (1., 1.))" ;;
assert_equal 1. (my_manhattan_distance (0., 1.) (1., 1.)) "1. (my_manhattan_distance (0., 1.) (1., 1.))" ;;
assert_equal 3. (my_manhattan_distance (0., -1.) (1., 1.)) "3. (my_manhattan_distance (0., -1.) (1., 1.))" ;;
assert_equal 5. (my_manhattan_distance (1., 1.) (3., 4.)) "5. (my_manhattan_distance (1., 1.) (3., 4.))" ;;
assert_equal 5. (my_manhattan_distance (3., 1.) (1., 4.)) "5. (my_manhattan_distance (3., 1.) (1., 4.))" ;;
assert_equal 9. (my_manhattan_distance (-3., 1.) (1., -4.)) "9. (my_manhattan_distance (-3., 1.) (1., -4.))" ;;

(* Question 7: my_euclidean_distance *)
print_endline ("--------------- my_euclidean_distance tests ---------------") ;;
assert_equal 0. (my_euclidean_distance (0.,0.) (0.,0.)) "0. (my_euclidean_distance (0.,0.) (0.,0.))" ;;
assert_equal 0. (my_euclidean_distance (1.,1.) (1.,1.)) "0. (my_euclidean_distance (1.,1.) (1.,1.))" ;;
assert_equal 4.24264068711928477 (my_euclidean_distance (5.,2.) (2.,5.)) "2. (my_euclidean_distance (3.,2.) (2.,3.))" ;;
assert_equal 5. (my_euclidean_distance (1.,1.) (4.,5.)) "5. (my_euclidean_distance (1.,1.) (4.,5.))" ;;

(* Question 8: my_range_sum *)
print_endline ("--------------- my_range_sum tests ---------------") ;;
assert_equal 0 (my_range_sum 2 1) "0 (my_range_sum 2 1)" ;;
assert_equal 2 (my_range_sum 2 2) "2 (my_range_sum 2 2)" ;;
assert_equal 5 (my_range_sum 2 3) "5 (my_range_sum 2 3)" ;;
assert_equal 9 (my_range_sum 2 4) "9 (my_range_sum 2 4)" ;;
assert_equal 10 (my_range_sum 1 4) "10 (my_range_sum 1 4)" ;;
assert_equal 55 (my_range_sum 1 10) "55 (my_range_sum 1 10)" ;;

(* Question 9: my_gcd *)
print_endline ("--------------- my_gcd tests ---------------") ;;
assert_equal 1 (my_gcd 2 17) "1 (my_gcd 2 17)" ;;
assert_equal 2 (my_gcd 2 18) "2 (my_gcd 2 18)" ;;
assert_equal 4 (my_gcd 8 100) "4 (my_gcd 8 100)" ;;
assert_equal 3 (my_gcd 3 9) "3 (my_gcd 3 9)" ;;
assert_equal 3 (my_gcd 9 3) "3 (my_gcd 9 3)" ;;
assert_equal 5 (my_gcd 15 25) "5 (my_gcd 15 25)" ;;
assert_equal 1 (my_gcd 9 10) "1 (my_gcd 9 10)" ;;

(* Question 10: my_even and my_odd *)
print_endline ("--------------- my_even and my_odd tests ---------------") ;;
assert_equal false (my_even 1) "false (my_even 1)" ;;
assert_equal true (my_odd 1) "true (my_odd 1)" ;;
assert_equal false (my_odd 4) "false (my_odd 4)" ;;
assert_equal true (my_even 4) "true (my_even 4)" ;;
assert_equal false (my_odd 0) "false (my_odd 0)" ;;
assert_equal true (my_even 0) "true (my_even 0)" ;;
assert_equal false (my_odd 100) "false (my_odd 100)" ;;
assert_equal true (my_even 100) "true (my_even 100)" ;;
assert_equal true (my_odd 101) "true (my_odd 101)" ;;
assert_equal false (my_even 101) "false (my_even 101)" ;;
