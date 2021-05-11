(* ----------------------------------------------------------------------
* Name: Zac Foteff
* File: hw9.ml
* Date: 4/20/21
* Desc: Function definitions and tests for a basic key-value pair
*        Algebraic type.
* ---------------------------------------------------------------------- *)


(* A key-value collection as a linked list. Allows duplicate keys. *)
type ('a,'b) kvlist = Node of 'a * 'b * ('a, 'b) kvlist
                     | Nil ;;

(* FUNCTION DEFINITIONS  *)

(* Q1: insert : 'a -> 'b -> ('a,'b) kvlist -> ('a,'b) kvlist *)
let insert k v xs =
  match xs with
  | Nil -> Node(k, v, Nil)
  | _ -> Node(k,v, xs)
;;

(* Q2:  remove: `a -> ('a,'b) kvlist -> ('a,'b) kvlist) *)
let rec remove k xs =
  match xs with
  | Nil -> Nil
  | Node (kp, _ , xsp) when kp = k -> remove(k)(xsp)
  | Node (kp, vp, xsp) -> Node(kp, vp, remove(k)(xsp))
;;

(* Q3:  size: ('a,'b) kvlist -> int *)
let rec size xs =
  match xs with
  | Nil -> 0
  | Node(_,_,xsp) -> 1 + size(xsp)
;;

(* Q4:  has_key: `a -> (`a, `b) kvlist -> `a list *)
let rec has_key k xs =
  match xs with
  | Nil -> false
  | Node (kp,v,xsp) when k=kp -> true
  | Node (_,_,xsp) -> has_key (k)(xsp)
;;

(* Q5:  keys: ('a,'b) kvlist -> `a list *)
let rec keys xs =
  match xs with
  | Nil -> []
  | Node(kp,_,xsp) -> kp :: keys(xsp)
;;

(* Q6:  values: ('a,'b) kvlist -> `b list *)
let rec values xs =
  match xs with
  | Nil -> []
  | Node(_,vp,xsp) -> vp :: values(xsp)
;;

(* Q7:  key_values:  `a -> ('a,'b) kvlist -> `b list *)
let rec key_values k xs =
  match xs with
  | Nil -> []
  | Node(kp,vp,xsp) when k=kp -> vp :: key_values(k)(xsp)
  | Node(_,_,xsp) -> key_values(k)(xsp)
;;

(* Q8:  combine: ('a,'b) kvlist -> ('a,'b) kvlist -> ('a,'b) kvlist *)
let rec combine xs ys =
  match xs with
  | Nil -> ys
  | Node(kp, vp, xsp) -> combine (xsp)(insert (kp)(vp)(ys))
;;

(* Q9:  invert: ('a,'b) kvlist -> ('b,'a) kvlist *)
let rec invert xs =
  match xs with
  | Nil -> Nil
  | Node(kp, vp, xsp) -> Node(vp, kp, invert(xsp))
;;

(* Q10: group: ('a,'b) kvlist -> ('a,'b list) kvlist *)
let rec group xs =
  match xs with
  | Nil -> Nil
  | Node(kp, vp, xsp) -> Node(kp, (key_values(kp)(xs)), group(remove(kp)(xsp)))
;;

(* Q11: kv_map: (`a -> `b -> (`c, `d)) -> (`a, `b) kvlist -> (`c, `d) kvlist *)
let rec kv_map f xs =
  match xs with
  | Nil -> Nil
  | Node(kp, vp, xsp) -> Node((fst) (f(kp)(vp)), (snd) (f(kp)(vp)), kv_map(f)(xsp))
;;

(* Q12: kv_filter: ('a -> 'b -> bool) -> (`a, `b) kvlist -> (`a, `b) kvlist *)
let rec kv_filter f xs =
  match xs with
  | Nil -> Nil
  | Node(kp, vp, xsp) when f(kp)(vp) -> Node(kp,vp,kv_filter(f)(xsp))
  | Node(_,_,xsp) -> kv_filter(f)(xsp)
;;

(* Q13: join: (`a, `b) kvlist -> (`a, `b) kvlist -> (`a, `b list) kvlist *)
let join xs ys =
  let rec unnest xs =
    match xs with
    | Nil -> Nil
    | Node(kp, vp, xsp) -> Node(kp, List.flatten (vp), unnest (xsp))

  in unnest(group(combine(group(xs))(group(ys))))
;;

(* Q14: count_keys_by_val: int -> (`a, `b) kvlist -> (`b, int) kvlist *)
let rec count_keys_by_val n xs =
  let ys = kv_filter (fun x y -> y >= n)(xs) in
  match ys with
  | Nil -> Nil
  | Node(kp, vp, xsp) -> kv_map(fun x y -> (x,y))(Node(kp, vp, xsp))
;;

(* TESTING *)

let assert_equal v1 v2 msg =
  let cond = v1 = v2 in
  assert (if not cond then print_endline ("TEST FAILED: " ^ msg) else print_endline("TEST PASSED: " ^ msg) ; cond)
;;

(* test data as needed *)
let c1 = Nil ;;
let c2 = Node ('a', 1, Nil) ;;
let c3 = Node ('b', 2, Node('c', 3, Nil)) ;;
let c4 = Node ('a', 1, c3) ;;
(* ... etc ... *)

(* Q1: insert tests *)
print_endline ("--------------- insert tests ---------------") ;;
assert_equal (Node('a',1,Nil)) (insert ('a') (1) (Nil)) ("Node('a',1,Nil) = insert ('a')(1)(Nil)") ;;
assert_equal (Node('a',1,Node('b',2,Node('c',3,Nil)))) (insert ('a') (1) (c3)) ("Node('a', 1, Node('b', 2, Node('c',3,Nil)))) = insert ('a') (1) (c3)") ;;

(* Q2: remove tests *)
print_endline ("--------------- remove tests ---------------") ;;
assert_equal (Nil) (remove ('a')(Nil)) ("Nil = remove 'a' Nil") ;;
assert_equal (Nil) (remove ('a')(Node('a',1,Nil))) ("Nil = remove ('a')(Node('a', 1, Nil))") ;;
assert_equal (Nil) (remove ('a')(Node('a',1,Node('a',2,Nil)))) ("Nil = remove ('a')(Node('a',1,Node('a',2,Nil)))") ;;
assert_equal (Node('b',1,Nil)) (remove ('a')(Node('b',1,Nil))) ("Node('b', 1, Nil) = remove ('a')(Node('b', 1, Nil))") ;;

(* Q3: size tests *)
print_endline ("--------------- size tests ---------------") ;;
assert_equal (0) (size Nil) ("0 = size Nil") ;;
assert_equal (1) (size (Node('a',1,Nil))) ("1 = size Node('a',1,Nil)") ;;
assert_equal (5) (size (Node('a',1,Node('b',2,Node('c',3,Node('d',4,Node('e',5,Nil))))))) ("5 = size (Node('a', 1, Node('b', 2, Node('c', 3, Node('d', 4, Node('e', 5, Nil))))))") ;;

(* Q4: has_key tests *)
print_endline ("--------------- has_key tests ---------------") ;;
assert_equal false (has_key ('a') (Nil)) ("false = has_key ('a') (Nil)") ;;
assert_equal true (has_key ('e') (Node('a',1,Node('b',2,Node('c',3,Node('d',4,Node('e',5,Nil))))))) ("true = has_key ('e') (Node('a',1,Node('b',2,Node('c',3,Node('d',4,Node('e',5,Nil)))))))") ;;

(* Q5: keys tests *)
print_endline ("--------------- keys tests ---------------") ;;
assert_equal [] (keys (Nil)) ("[] = keys (Nil)") ;;
assert_equal ['b';'a'] (keys (Node('b',2,Node('a',1,Nil)))) ("['b';'a'] = keys (Node('b',2,Node('a',1,Nil)))") ;;
assert_equal ['a';'b';'c';'d';'e'] (keys (Node('a',1,Node('b',2,Node('c',3,Node('d',4,Node('e',5,Nil))))))) ("['a';'b';'c';'d';'e'] = keys (Node('a', 1, Node('b', 2, Node('c', 3, Node('d', 4, Node('e', 5, Nil))))))") ;;

(* Q6: values tests *)
print_endline ("--------------- values tests ---------------") ;;
assert_equal [] (values (Nil)) ("[] = values (Nil)") ;;
assert_equal [2;1] (values (Node('b',2,Node('a',1,Nil)))) ("[2;1] = values (Node('b',2,Node('a',1,Nil)))") ;;
assert_equal [1;2;3;4;5] (values (Node('a',1,Node('b',2,Node('c',3,Node('d',4,Node('e',5,Nil))))))) ("[1;2;3;4;5] = values (Node('a', 1, Node('b', 2, Node('c', 3, Node('d', 4, Node('e', 5, Nil))))))") ;;

(* Q7: key_values tests *)
print_endline ("--------------- key_values tests ---------------") ;;
assert_equal [] (key_values('a')(Nil)) ("[] = key_values('a')(Nil)") ;;
assert_equal [1] (key_values('a')(Node('a',1,Nil))) ("[1] = key_values('a')(Node('a',1,Nil))") ;;
assert_equal [1;2;3] (key_values('a')(Node('a',1,Node('a',2,Node('a',3,Nil))))) ("[1;2;3] = key_values('a')(Node('a', 1, Node('a', 2, Node('a', 3, Nil))))") ;;
assert_equal [1;1;1] (key_values('a')(Node('a',1,Node('a',1,Node('a',1,Nil))))) ("[1;1;1] = key_values('a')(Node('a', 1, Node('a', 1, Node('a', 1, Nil))))") ;;
assert_equal [] (key_values('d')(Node('a',1,Node('b',2,Node('c',3,Nil))))) ("[] = key_values('d')(Node('a', 1, Node('b', 2, Node('c', 3, Nil))))") ;;

(* Q8:  combine tests *)
print_endline ("--------------- combine tests ---------------") ;;
assert_equal (Node('a',1,Nil)) (combine (Node('a',1,Nil))(Nil)) ("Node('a', 1, Nil) = combine (Node('a', 1, Nil))(Nil)") ;;
assert_equal (Node('a',1,Nil)) (combine (Nil)(Node('a',1,Nil))) ("Node('a', 1, Nil) = combine (Nil)(Node('a', 1, Nil))") ;;
assert_equal (Node('a',1,Node('b',2,Nil))) (combine (Node('a',1,Nil)) (Node('b',2,Nil))) ("Node('a',1,Node('b',2,Nil)) = combine (Node('a',1,Nil)) (Node('b',2,Nil))") ;;

(* Q9:  invert tests *)
print_endline ("--------------- invert tests ---------------") ;;
assert_equal (Node(1,'a',Nil)) (invert (Node('a',1,Nil))) ("Node(1, 'a', Nil) = invert (Node('a', 1, Nil))") ;;
assert_equal (Node(1,'a',Node(2,'b',Node(3,'c',Nil)))) (invert (Node('a',1,Node('b',2,Node('c',3,Nil))))) ("Node(1, 'a', Node(2, 'b', Node(3, 'c', Nil))) = invert (Node('a', 1, Node('b', 2, Node('c', 3, Nil))))") ;;

(* Q10: group tests *)
print_endline ("--------------- group tests ---------------") ;;
assert_equal (Node('a',[1;2],Nil)) (group (Node('a',1,Node('a',2,Nil)))) ("Node('a', [1;2], Nil) = group (Node('a', 1, Node('a', 2, Nil)))") ;
assert_equal (Node('a',[1],Node('b',[2],Nil))) (group (Node('a',1,Node('b',2,Nil)))) ("Node('a',[1],Node('b',[2],Nil))) = group (Node('a',1,Node('b',2,Nil)))") ;;
assert_equal (Node('a',[1],Node('b',[2;3],Nil))) (group (Node('a',1,Node('b',2,Node('b',3,Nil))))) ("Node('a',[1],Node('b',[2;3],Nil))) = group (Node('a',1,Node('b',2,Node('b',3,Nil))))") ;;
assert_equal (Node('a',[1;2;3;4;5],Nil)) (group (Node('a',1,Node('a',2,Node('a',3,Node('a',4,Node('a',5,Nil))))))) ("Node('a', [1;2;3;4;5], Nil) = group (Node('a',1,Node('a',2,Node('a',3,Node('a',4,Node('a',5,Nil))))))") ;;

(* Q11: kv_map tests *)
print_endline ("--------------- kv_map tests ---------------") ;;

(* Map function for testing kv_map -> my_map_fun : char -> int -> int * bool *)
(* Outputs ascii representation of k and a bool checking if v is less that 5 *)
let my_map_fun k v =
  let new_k = Char.code (k) - 96
  and new_v = v < 5
  in (new_k, new_v)
;;

assert_equal (Node(1, true, Nil)) (kv_map(my_map_fun)(Node('a',1,Nil))) ("Node(1, true, Nil) = kv_map(my_map_fun)(Node('a',1,Nil))") ;;
assert_equal (Node(1, true, Node(2, false, Node(6, true, Nil)))) (kv_map(my_map_fun)(Node('a',1,Node('b', 10, Node('f', 3, Nil))))) ("Node(1, true, Node(2, false, Node(6, true, Nil))) = kv_map(my_map_fun)(Node('a',1,Node('b', 10, Node('f', 3, Nil)))))") ;;

(* Q12: kv_filter tests *)
print_endline ("--------------- kv_filter tests ---------------") ;;

(* Filter function for testing kv_filter -> my_filter_fun : char -> int -> bool *)
(* Returns true if key = 'a' or value < 5, false otherwise *)
let my_filter_fun k v = (k == 'a') || (v < 5) ;;

assert_equal (Node('a', 5, Nil)) (kv_filter (my_filter_fun)(Node('a', 5, Nil))) ("Node('a', 5, Nil) = kv_filter (my_filter_fun)(Node('a', 5, Nil))") ;;

assert_equal (Nil) (kv_filter (my_filter_fun)(Node('b', 5, Nil))) ("Nil = kv_filter (my_filter_fun)(Node('b', 5, Nil))") ;;

(* Q13: join tests *)
print_endline ("--------------- join tests ---------------") ;;
assert_equal (Nil) (join (Nil)(Nil)) ("Nil = join (Nil)(Nil)") ;;
assert_equal (Node('a',[1],Nil)) (join (Node('a',1,Nil))(Nil)) ("Node('a', 1, Nil) = join (Node('a', 1, Nil)(Nil))") ;;
assert_equal (Node('a',[1],Nil)) (join (Nil)(Node('a',1,Nil))) ("Node('a', 1, Nil) = join (Nil)(Node('a', 1, Nil))") ;;
assert_equal (Node('a',[1;2],Nil)) (join (Node('a',1,Nil))(Node('a',2,Nil))) ("Node('a', [1;2], Nil) = join (Node('a', 1, Nil))(Node('a', 2, Nil))") ;;
assert_equal (Node('a',[1],Node('b',[2],Nil))) (join (Node('a',1,Nil))(Node('b',2,Nil))) ("Node('a', [1], Node('b', [2], Nil)) = join (Node('a', 1, Nil))(Node('b', 2, Nil))") ;;
assert_equal (Node('a',[1],Node('b',[2;3],Nil))) (join (Node('a',1,Nil))(Node('b',2,Node('b',3,Nil)))) ("Node('a', [1], Node('b', [2;3], Nil)) = join (Node('a', 1, Nil))(Node('b', 2, Node('b', 3, Nil)))") ;;
assert_equal (Node('a',[1;4],Node('b',[2;3],Nil))) (join (Node('a',1,Nil))(Node('b',2,Node('b',3,Node('a',4,Nil))))) ("Node('a', [1;4], Node('b', [2;3], Nil)) = join (Node('a', 1, Nil))(Node('b', 2, Node('b', 3, Node('a', 4, Nil))))") ;;

(* Q14: count_keys_by_val tests *)
print_endline ("--------------- count_keys_by_val tests ---------------") ;;
