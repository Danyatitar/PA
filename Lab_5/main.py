import random

SCOUTS = 5
BEST_SCOUTS = 4
RANDOM_SCOUTS = SCOUTS - BEST_SCOUTS
FORAGERS = 45



class Knapsack:
    def __init__(self, items):
        self.items = items
        self.value = calc_value(self)
        self.amount=len(items)
        self.weight=calc_weight(self)

class Item:
    def __init__(self, value, weight):
        self.value = value
        self.weight = weight

    def __eq__(self, other): 
        if not isinstance(other, Item):
            return NotImplemented

        return self.value == other.value and self.weight == other.weight
# Генеруємо предмети
def generate_items(n):
    arr=[]
    
    for i in range(n):
        arr.append(Item(random.randint(2,30),random.randint(1,20)))

    return arr
# випадкове пакування рюкзака
def random_knapsack(W,arr):
    unpacked=True
    final=[]
    exist=[]
    finalvalue = 0.0
    while(unpacked):
        index=random.randint(1,len(arr)-1)
        if not(index in exist):
          W -= arr[index].weight
          if(W<0):
            W += arr[index].weight
            unpacked=False
          else:
            finalvalue += arr[index].value
            final.append(arr[index])
            exist.append(index)
    return Knapsack(final)
# жадібне пакування рюкзака
def greedy(W, arr):
    
    arr.sort(key=lambda x: (x.value/x.weight), reverse=True)   
    begin=W
    finalvalue = 0.0
    final=[]

    for item in arr:
        final.append(item)
        if item.weight <= W:
            W -= item.weight
            finalvalue += item.value
            
        else:
            finalvalue += item.value * W / item.weight
            break
     
    return Knapsack(final)
# Рахуємо цінність всіх предметів у рюкзаку
def calc_value(knapsack):
    total=0
    for item in knapsack.items:
        total=total+item.value
    return total
# Рахуємо вагу всіх предметів у рюкзаку
def calc_weight(knapsack):
    total=0
    for item in knapsack.items:
        total=total+item.weight
    return total
# Пробуємо замінити один предмет з рюкзаку на предмет, що лишився
def change_item(knapsack,arr,W,index):
    items=knapsack.items
    left_items=[]
    W_left=W-knapsack.weight
    max_val=knapsack.value

    for item in arr:
        if not(item in items):
            left_items.append(item)

    items.sort(key=lambda x: (x.value/x.weight), reverse=False)
    left_items.sort(key=lambda x: (x.value/x.weight), reverse=True)

    if((items[index].value/items[index].weight)<(left_items[0].value/left_items[0].weight)):
        W_left=W_left+items[index].weight
        begin_val=max_val-items[index].value
        best_item_index=0
        find=False
        for i in range(len(left_items)):
            if(left_items[i].weight<W_left and begin_val+left_items[i].value>=max_val):
                max_val=begin_val+left_items[i].value
                best_item_index=i
                find=True
        if(find):
            W_left=W_left-left_items[best_item_index].weight
            items[index]=left_items[best_item_index]
            left_items.remove(left_items[best_item_index])
      
    if(W_left>0):
        for item in left_items:
            if(item.weight<W_left):
                items.append(item)
                W_left=W_left-item.weight
                max_val=max_val+item.value
                left_items.remove(item)

    knapsack.value=max_val
    knapsack.weight=W-W_left
    knapsack.items=items
    return knapsack
# Генеруємо випадкових розвідників
def create_random_scouts(arr, W,scouts, n):
    while len(scouts) < n:
        scout=random_knapsack(W,arr)
        if scout not in scouts:
            scouts.append(scout)
# Генеруємо елітних розвідників
def create_best_scouts(arr, W,scouts, n):
    while len(scouts) < n:
        scout=greedy(W,arr)
        if scout not in scouts:
            scouts.append(scout)
# Пошук рішень
def local_search(arr,W,scouts,foragers):
    scout_result=[]
    scouts.sort(key=lambda x: (x.value), reverse=True)
    for scout in scouts:
        n=scout.amount
        if(foragers<n):
            n=foragers
        for i in range(n):
            scout=change_item(scout,arr,W,i)
            scout_result.append(scout)
    return scout_result
# Знаходимо найкращу цінність рюкзака
def get_best(scouts):
    scouts.sort(key=lambda x: (x.value), reverse=True)
    return scouts[0]


if __name__ == "__main__":
    W = 500
    arr = generate_items(100)
    scouts=[]
    best_result=Knapsack([Item(0,0)])
    create_best_scouts(arr,W,scouts,BEST_SCOUTS)
    create_random_scouts(arr,W,scouts,SCOUTS)
    for i in range(1000):
        if  (i % 20==0 and i!=0):
            print(f"i: {i}, max value: {best_result.value}")
        if(i==0):
            best=get_best(scouts)
            print(f"Begin Knapsack\n i: {i},  value: {best.value}")
        results = local_search(arr,W,scouts,FORAGERS)
        # print(get_best(results).value)
        if(get_best(results).value>best_result.value):
            best_result=get_best(results)
        scouts=[]
        create_best_scouts(arr,W,scouts,BEST_SCOUTS)
        create_random_scouts(arr,W,scouts,SCOUTS)
    print(f"Best knapsack:\n Weight: {best_result.weight} \n Value: {best_result.value}")