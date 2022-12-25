import Control.Monad (forM_)
import Data.List (elemIndex,unfoldr)
import Data.Maybe (fromJust)

day25 :: [String] -> String
day25 = reverse . unfoldr op . sum . map decypher
  where digits = "=-012"
        decypher = foldl (\res c -> res * 5 + (fromJust $ elemIndex c digits) - 2) 0
        op 0 = Nothing
        op n = Just (digits !! (m + 2), (n - m) `div` 5)
          where m = ((n `mod` 5) + 2) `mod` 5 - 2 -- map [0..4] to [0,1,2,-2,-1]

main :: IO ()
main = forM_ ["input/25test.txt", "input/25full.txt"] $ \filename -> do
        input <- lines <$> readFile filename
        print $ day25 input
