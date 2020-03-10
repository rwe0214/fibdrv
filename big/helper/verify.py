import unittest
import sys

class TestMultiplication(unittest.TestCase):
    def test_mul(self):
        f0 = 0
        f1 = 1
        i=2
        c_out = open(r'./helper/out.txt')
        n = int(c_out.readline())
        res_cpp = int(c_out.readline())
        

        while i<n+1:
            res = f0 + f1
            f0 = f1
            f1 = res
            i+=1

        c_out.close()
        self.assertEqual(res, res_cpp)
        
        
if __name__ == '__main__':
    unittest.main()
    input()
