(*
	Name: Zac Foteff
        File: hw8.ml
        Date: 4/13/2021
        Desc: Multiple function implementations for lists with pattern matching
*)

(* ------------- Function Implementations --------------- *)
(* Question 1: my_rev - 'a list -> 'a list = <fun> *)
	let rec my_rev xs =
		match xs with
		| [] -> []
		| h::t -> my_rev t @ [h]
;;

(* Question 2: my_last - 'a list -> 'a = <fun> *)
let rec my_last xs =
	match xs with
	| [] -> failwith "Empty List"
	| [x] -> x
	| _::t -> my_last(t)
;;

(* Question 3: my_init - 'a list -> 'a list = <fun> *)
let rec my_init xs =
	match xs with
	|	[] -> []
	|	[x] -> []
	| h::t -> h :: my_init(t)
;;

(*	Question 4: my_mem - 'a -> 'a list -> bool = <fun> *)
let rec my_mem num xs =
	match xs with
	| [] -> false
	| h::t when h == num -> true
	| _::t -> my_mem(num)(t)
;;

(*	Question 5: my_replace - 'a * 'a -> 'a list -> 'a list = <fun> *)
let rec my_replace v (xs) =
	match xs with
	| [] -> []
	| h::t when h == (fst v) -> (snd v) :: my_replace (v)(t)
	| h::t -> h::my_replace(v)(t)
;;

(*	Question 6: my_replace_all - ('a * 'a) list -> 'a list -> 'a list = <fun> *)
let rec my_replace_all rs xs =
	match rs with
	| [] -> xs
	| h::t -> my_replace_all(t)(my_replace(h)(xs))
;;

(*	Question 7: my_element_sum - int -> int list -> int = <fun> *)
let my_element_sum v xs =
	match xs with
	| [] -> 0
	| h::t when h == v-> v + my_element_sum(v)(t)
	| _::t -> 0 + my_element_sum(v)(t)
;;

(*	Question 8: my_rem_dups	- 'a list -> 'a list = <fun> *)
let my_rem_dups xs =
	let rec loop dups xs =
		match xs with
		| [] -> dups
		| h::t when my_mem(h)(dups) -> loop(dups)(t)
		| h::t -> loop (h::dups)(t)
	in loop ([])(xs)
;;

(*	Question 9: my_min - 'a list -> 'a = <fun> *)
let my_min xs =
	let rec find_min min xs =
		match xs with
		| [] -> min
		| h::t when h < min -> find_min (h)(t)
		| _::t -> find_min (min)(t)
	in match xs with
	| [] -> failwith "Empty List"
	| h::t -> find_min (h)(xs)
;;

(*	Question 10: merge_sort Not Working	*)


(*	----------- Testing -------------	*)
let assert_equal v1 v2 msg =
  let cond = v1 = v2 in
  assert (if not cond then print_endline ("TEST FAILED: " ^ msg) else print_endline("TEST PASSED: " ^ msg) ; cond)
;;

(* Question 1: my_rev - 'a list -> 'a list = <fun> *)
print_endline ("--------------- my_rev tests ---------------") ;;
assert_equal [3;2;1] (my_rev [1;2;3]) "[3;2;1] = my_rev [1;2;3]" ;;
assert_equal [8;7;6;5;4;3;2;1] (my_rev [1;2;3;4;5;6;7;8]) "[8;7;6;5;4;3;2;1] = my_rev [1;2;3;4;5;7;8]" ;;
assert_equal ['c';'b';'a'] (my_rev ['a';'b';'c']) "['c';'b';'a'] = my_rev ['a';'b';'c']" ;;

(* Question 2: my_last - 'a list -> 'a = <fun> *)
print_endline ("--------------- my_min tests ---------------") ;;
assert_equal 3 (my_last [1;2;3]) "3 = my_last [1;2;3]" ;;
assert_equal 1 (my_last [1;0;1]) "1 = my_last [1;0;1]" ;;
assert_equal 0 (my_last [1;1;0]) "0 = my_last [1;1;0]" ;;
assert_equal 1 (my_last [0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;1]) "1 = my_last [0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;1]" ;;
assert_equal 1.5 (my_last [1.;1.1;1.2;1.3;1.4;1.5]) "1.5 = my_last [1.;1.1;1.2;1.3;1.4;1.5]" ;;
assert_equal () (try my_last [] with _ -> ()) "my_last []";;

(* Question 3: my_init - 'a list -> 'a list = <fun> *)
print_endline ("--------------- my_init tests ---------------") ;;
assert_equal [1;2] (my_init [1;2;3]) "[1;2] = my_init [1;2;3]" ;;
assert_equal [3;2] (my_init [3;2;1]) "[3;2] = my_init [3;2;1]" ;;
assert_equal [1;2;3;4;5;6;7] (my_init [1;2;3;4;5;6;7;8]) "[1;2;3;4;5;6;7] = my_init [1;2;3;4;5;6;7;8]" ;;

(*	Question 4: my_mem - 'a -> 'a list -> bool = <fun> *)
print_endline ("--------------- my_mem tests ---------------") ;;
assert_equal true (my_mem 1 [1;2;3]) "true = my_mem 1 [1;2;3]" ;;
assert_equal false (my_mem 4 [1;2;3]) "false = my_mem 4 [1;2;3]" ;;
assert_equal true (my_mem 1 [0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;1]) "true = my_mem 1 [1;2;3]" ;;

(*	Question 5: my_replace - 'a * 'a -> 'a list -> 'a list = <fun> *)
print_endline ("--------------- my_replace tests ---------------") ;;
assert_equal [1;8;3;8] (my_replace (2,8) [1;2;3;2]) "[1;8;3;8] = my_replace (2,8) [1;8;3;8]" ;;
assert_equal [2;2;2;2] (my_replace (1,2) [1;1;1;1]) "[2;2;2;2] = my_replace (1,2) [1;1;1;1]" ;;
assert_equal [1;2;3;4] (my_replace (5,1) [1;2;3;4]) "[1;2;3;4] = my_replace (5,1) [1;2;3;4]" ;;

(*	Question 6: my_replace_all - ('a * 'a) list -> 'a list -> 'a list = <fun> *)
print_endline ("--------------- my_replace_all tests ---------------") ;;
assert_equal [3;3;3;4] (my_replace_all [(1,2); (2,3)] [1;2;3;4]) "[3;3;3;4] = my_replace_all [(1,2); (2,3)] [1;2;3;4]" ;;
assert_equal [2;2;1;4] (my_replace_all [(1,2); (3,1)] [1;2;3;4]) "[2;2;1;4] = my_replace_all [(1,2); (3,1)] [2;2;1;4]" ;;
assert_equal ['b';'b';'d';'d'] (my_replace_all [('a','b'); ('c','d')] ['a';'b';'c';'d']) "['b';'b';'d';'d'] = my_replace_all [('a','b'); ('c','d')] ['a';'b';'c';'d']" ;;

(*	Question 7: my_element_sum - int -> int list -> int = <fun> *)
print_endline ("--------------- my_element_sum tests ---------------") ;;
assert_equal 10 (my_element_sum 10 [15;10;25]) "10 = my_element_sum 10 [15;10;25]" ;;
assert_equal 12 (my_element_sum 3 [3;2;3;2;3;4;3]) "12 = my_element_sum 3 [3;2;3;2;3;4;3]" ;;
assert_equal 0 (my_element_sum 3 []) "0 = my_element_sum 3 []" ;;
assert_equal 0 (my_element_sum 3 [1;2;4;5;6;7;8;9]) "0 = my_element_sum 3 [1;2;4;5;6;7;8;9]" ;;

(*	Question 8: my_rem_dups	- 'a list -> 'a list = <fun> *)
print_endline ("--------------- my_rem_dups tests ---------------") ;;
assert_equal ['c';'b';'a'] (my_rem_dups ['a';'b';'a';'c';'b';'a']) "['c';'b';'a'] = my_rem_dups ['a';'b';'a';'c';'b';'a']" ;;
assert_equal [4;3;2;1] (my_rem_dups [1;2;3;4;3;2;1]) "[4;3;2;1] = my_rem_dups [1;2;3;4;3;2;1]" ;;
assert_equal [1;2;3;4] (my_rem_dups [4;3;2;1]) "[1;2;3;4] = my_rem_dups [4;3;2;1]" ;;
assert_equal [] (my_rem_dups []) "[] = my_rem_dups []" ;;

(*	Question 9: my_min - 'a list -> 'a = <fun>	*)
print_endline ("--------------- my_min tests ---------------") ;;
assert_equal 1 (my_min [1;2;3]) "1 = my_min [1;2;3]" ;;
assert_equal 1 (my_min [3;2;1]) "1 = my_min [3;2;1]" ;;
assert_equal 12 (my_min [34;81;36;100;24;76;12;14;65]) "12 = my_min [34;81;36;100;24;76;12;14;65]" ;;
assert_equal () (try my_min [] with _ -> ()) "my_min []";;
