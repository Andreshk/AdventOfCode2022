import Control.Applicative (liftA2)
import Text.Read (readMaybe)

data Tree = Leaf String Int
          | Node Char Tree Tree

build :: String -> [String] -> Tree
build key lines = case readMaybe (drop 6 line) :: Maybe Int
    of (Just val) -> Leaf key val
       Nothing -> Node op (build lhs lines) (build rhs lines)
  where line = head $ filter ((==key) . take 4) lines -- O(n) :(
        [_, lhs, [op], rhs] = words line

fromChar :: Char -> Int -> Int -> Int
fromChar '+' = (+); fromChar '-' = (-)
fromChar '*' = (*); fromChar '/' = div

-- Part1: just evaluate the tree
eval :: Tree -> Int
eval (Leaf _ val) = val
eval (Node op lhs rhs) = (fromChar op) (eval lhs) (eval rhs)

-- Given an operation, one of the operands and
-- the goal, returns the value of the other operand
findL, findR :: Char -> Int -> Int -> Int
findL '+' = (-); findL '-' = (+)
findL '*' = div; findL '/' = (*)
findR '+' = flip (-); findR '-' = (-)
findR '*' = flip div; findR '/' = div

-- Part2: finds the value of "humn" leaf such that both subtrees eval to the same value
solve :: Tree -> Int
solve (Node _ lhs rhs) = case (eval2 lhs, eval2 rhs)
    of (Just lhsRes, Nothing) -> find lhsRes rhs
       (Nothing, Just rhsRes) -> find rhsRes lhs
  where -- Returns Nothing if "humn" is among the leaves, equivalent to (Just . eval) otherwise
        eval2 :: Tree -> Maybe Int
        eval2 (Leaf "humn" _) = Nothing
        eval2 (Leaf _ val) = Just val
        eval2 (Node op lhs rhs) = (liftA2 $ fromChar op) (eval2 lhs) (eval2 rhs)
        -- Given a goal & tree with "humn" as leaf, finds the value of this leaf
        find :: Int -> Tree -> Int
        find goal (Leaf "humn" _) = goal
        find goal (Node op lhs rhs) = case (eval2 lhs, eval2 rhs)
            of (Just lhsRes, Nothing) -> find (findR op lhsRes goal) rhs
               (Nothing, Just rhsRes) -> find (findL op goal rhsRes) lhs

main :: IO ()
main = do
    t1 <- build "root" . lines <$> readFile "input/21test.txt"
    t2 <- build "root" . lines <$> readFile "input/21full.txt"
    print $ (eval t1, solve t1) -- (152,301)
    print $ (eval t2, solve t2) -- (93813115694560,3910938071092)
