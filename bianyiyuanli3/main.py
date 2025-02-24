import re
import copy
from prettytable import PrettyTable


# 按终结符和非终结符遍历
def match_strings(A, input_str):
    # 优先匹配最长(A'和A识别成A')
    A = sorted(A, key=lambda x: len(x), reverse=True)
    pattern = '|'.join(map(re.escape, A))
    matches = re.findall(pattern, input_str)
    return matches


# 按终结符非终结符整体字符串倒序
def reverse_by_set(A, input_str):
    result = []
    i = len(input_str)
    while i > 0:
        for word in reversed(A):
            word_len = len(word)
            if i >= word_len and input_str[i - word_len:i] == word:
                result.append(word)
                i -= word_len
                break
        else:
            i -= 1
    return ''.join(result)


# 可视化输出
class draw_grammer:
    def draw_grammer(grammer, vn, descrpition):
        print_content = PrettyTable(['编号', '左部', '右部', '产生式'])
        idx = 1
        for i in vn:
            for j in grammer[i]:
                print_content.add_row([idx, i, j, i + '->' + j])
                idx += 1
        print('\n\n' + descrpition + ':\n', print_content)


# 消除左递归
class EliminateLeftRecursion:
    def __init__(self, grammer, vn):
        self.grammer = grammer
        self.vn = vn

    # 消除间接左递归
    def remove_left_recursion(self):
        new_grammer = copy.deepcopy(self.grammer)
        new_vn = copy.deepcopy(self.vn)

        # 两层循环暴露直接左递归
        for i in range(len(self.vn)):
            for j in range(0, i):
                # 检查是否有间接左递归
                if self.has_indirect_left_recursion(self.vn[i], self.vn[j], new_grammer):
                    new_grammer = self.convert(self.vn[i], self.vn[j], new_grammer)

            new_grammer, new_vn = self.clean_direct_recursion(self.vn[i], new_grammer, new_vn)

        return new_grammer, new_vn

    # 检查是否存在间接左递归
    def has_indirect_left_recursion(self, ch_i, ch_j, grammer):
        # 通过深度优先搜索来检查 ch_i 是否通过 ch_j 间接递归回自身
        visited = set()
        stack = [ch_j]  # 从 ch_j 开始，检查是否能回到 ch_i

        while stack:
            current = stack.pop()
            if current == ch_i:  # 如果发现间接左递归
                return True
            if current not in visited:
                visited.add(current)
                if current in grammer:  # 查找所有生成 current 的非终结符
                    for item in grammer[current]:
                        if item:  # 不为空产生式
                            stack.append(item[0])  # 只考虑产生式的第一个符号，判断是否是非终结符

        return False

    # 产生式右部非终结符转终结符
    def convert(self, ch_i, ch_j, grammer):
        rules = copy.deepcopy(grammer)
        for key in grammer.keys():
            for item_i in grammer[key]:
                if ch_i == key and ch_j == item_i[0]:
                    rules[key].remove(item_i)
                    for item_j in grammer[ch_j]:
                        rules[key].append(item_j + item_i[1:])
        return rules

    # 消除直接左递归
    def clean_direct_recursion(self, ch_i, grammer, new_vn):
        ch = ch_i + "'"
        flag = 0
        rules = copy.deepcopy(grammer)

        for key in grammer.keys():
            for item_i in grammer[key]:
                if ch_i == key and ch_i == item_i[0]:
                    flag = 1
                    # 添加新非终结符
                    if ch not in rules.keys():
                        rules[ch] = []
                    rules[ch].append(item_i[1:] + ch)
                    rules[key].remove(item_i)

        # 不存在左递归，直接返回
        if flag == 0:
            return rules, new_vn

        # 处理剩余的产生式
        for key in grammer.keys():
            for item_i in grammer[key]:
                if ch_i == key and ch_i != item_i[0]:
                    if ch not in rules.keys():
                        rules[ch] = []
                    rules[ch_i].append(item_i + ch)
                    rules[key].remove(item_i)

        # 添加新非终结符空串产生式
        rules[ch].append('ε')
        new_vn.append(ch)

        return rules, new_vn


# 提取左公因子
class ExtractCommonFactors:
    def __init__(self, grammer, vn):
        self.grammer = grammer
        self.vn = vn

    # 获取最长公共前缀
    def LCP(self, i, j, rules):
        strs = [rules[i], rules[j]]
        res = ''
        for each in zip(*strs):
            if len(set(each)) == 1:
                res += each[0]
            else:
                return res
        return res

    # 获取公共前缀索引
    def get_lcp_res(self, key):
        res = {}
        rules = self.grammer[key]
        for i in range(len(rules)):
            for j in range(i + 1, len(rules)):
                temp = self.LCP(i, j, rules)
                if temp not in res.keys():
                    res[temp] = set()
                res[temp].add(i)
                res[temp].add(j)
        # 去空串前缀
        if '' in res.keys():
            res.pop('')
        return res

    def remove_common_factor(self):
        keys = list(self.grammer.keys())
        for key in keys:
            while (True):
                res = self.get_lcp_res(key)
                # 直到没有公共前缀
                if (res == {}):
                    break
                dels = []  # 存即将删除的串
                lcp = list(res.keys())[0]  # 每次取一个公共前缀
                ch = key + "'"
                if ch not in self.vn:
                    self.vn.append(ch)
                # 遍历要消除公共因子的元素下标
                for i in res[lcp]:
                    string = self.grammer[key][i]
                    dels.append(string)
                    string = string.lstrip(lcp)
                    if string == '':
                        string += 'ε'
                    if ch not in self.grammer.keys():
                        self.grammer[ch] = []
                    # 加入新产生式
                    self.grammer[ch].append(string)
                # 删去原来产生式
                for string in dels:
                    self.grammer[key].remove(string)
                self.grammer[key].append(lcp + ch)
        return self.grammer, self.vn


# 文法分析
class LL1_analysis:
    def __init__(self, Gram):
        # 终结符 非终结符 分析表元素 $+开始符号
        self.vt, self.vn, self.analysis_table, self.stack_str = self.init_all_(g=Gram)
        self.ptr = 0

    def init_all_(self, g):
        # 读取文法
        grammer_list = {}  # 非终结符：产生式
        vn_list = []  # 非终结符
        for line in re.split('\n', g):
            # 去空格
            line = "".join([i for i in line if i not in ['', ' ']])
            if '->' in line:
                if line.split('->')[0] not in vn_list:
                    vn_list.append(line.split('->')[0])
                for i in line.split('->')[1].split('|'):
                    if grammer_list.get(line.split('->')[0]) is None:
                        grammer_list[line.split('->')[0]] = []
                        grammer_list[line.split('->')[0]].append(i)
                    else:
                        grammer_list[line.split('->')[0]].append(i)
        draw_grammer.draw_grammer(grammer=grammer_list, vn=vn_list, descrpition='输入的文法')

        # 消除左递归
        # print('产生式：', grammer_list)
        # print('非终结符：', vn_list)
        eliminate_left_recursion = EliminateLeftRecursion(grammer=grammer_list, vn=vn_list)
        new_grammer, new_vn = eliminate_left_recursion.remove_left_recursion()
        draw_grammer.draw_grammer(grammer=new_grammer, vn=new_vn, descrpition='消除左递归')

        # 提取左公因子
        extractcommonfactors = ExtractCommonFactors(grammer=new_grammer, vn=new_vn)
        new_grammer, new_vn = extractcommonfactors.remove_common_factor()
        draw_grammer.draw_grammer(grammer=new_grammer, vn=new_vn, descrpition='提取公因子')

        only_grammer = []
        new_vt = []
        for i in new_vn:
            for j in new_grammer[i]:
                only_grammer.append(i + '->' + j)

                for t in j:  # 获取当前的所有的终结符
                    if t not in new_vt and t not in new_vn and t != "ε" and t != "'":
                        # print(t)
                        new_vt.append(t)
        new_vt.append('$')
        # print('\n\n消除文法左递归的文法的非终结符:',new_vn,
        #       '\n\n消除文法左递归的文法的终结符:', new_vt)

        # FIRST集和FOLLOW集
        FIRST, FOLLOW = self.get_first_and_follow_set(grammars=only_grammer, vn=new_vn, vt=new_vt)
        print('\n\n文法的FIRST集:')
        for i, j in FIRST.items():
            if j:  # 检查j是否为空
                str = j[0]
                for temp in j[1:]:
                    str = str + ',' + temp
                print("FIRST(" + i + ")" + " = {" + str + "}")
            else:
                print("FIRST(" + i + ")" + " = {}")

        print('\n\n文法的FOLLOW集:')
        for i, j in FOLLOW.items():
            if j:  # 检查j是否为空
                str = j[0]
                for temp in j[1:]:
                    str = str + ',' + temp
                print("FOLLOW(" + i + ")" + " = {" + str + "}")
            else:
                print("FOLLOW(" + i + ")" + " = {}")

        # 分析表
        analysis_table = [[None] * (1 + len(new_vt)) for row in range(1 + len(new_vn))]
        analysis_table[0][0] = ' '
        for i in range(len(new_vt)):
            analysis_table[0][i + 1] = new_vt[i]
        for i in range(len(new_vn)):
            analysis_table[i + 1][0] = new_vn[i]
        for i in range(len(new_vn)):
            for t in new_grammer[new_vn[i]]:  # 遍历该文法的所有产生式
                if t == 'ε':  # 如果是ε，对应在FOLLOW集中的终结符位置填上ε
                    for j in range(len(new_vt)):  # 遍历所有的终结符
                        if new_vt[j] in FOLLOW[new_vn[i]]:  # FOLLOW[part_begin]为当前非终结符的FOLLOW集
                            # 如果分析表该位置为空，则填入ε
                            if analysis_table[i + 1][j + 1] is None:
                                analysis_table[i + 1][j + 1] = 'ε'
                else:
                    first_found = False  # 用于标记是否已经找到有效的FIRST项
                    for symbol in t:  # 遍历产生式右侧的每个符号
                        if symbol in new_vt:  # 如果是终结符
                            # 将该符号填入对应位置
                            j = new_vt.index(symbol)
                            if analysis_table[i + 1][j + 1] is None:
                                analysis_table[i + 1][j + 1] = t
                            first_found = True
                            break  # 终结符就直接填入，并停止检查其他符号
                        else:  # 如果是非终结符
                            # 使用该非终结符的FIRST集
                            for first_symbol in FIRST[symbol]:
                                if first_symbol != 'ε':  # 只处理非ε项
                                    j = new_vt.index(first_symbol)
                                    if analysis_table[i + 1][j + 1] is None:
                                        analysis_table[i + 1][j + 1] = t
                            # 如果该非终结符的FIRST集包含ε，需要继续检查后面的符号
                            if 'ε' in FIRST[symbol]:
                                continue
                            else:
                                first_found = True
                                break  # 如果FIRST集没有包含ε，停止检查后面的符号

                    # 如果右侧符号都能推导出ε，则检查FOLLOW集并填充
                    if not first_found:
                        for j in range(len(new_vt)):
                            if new_vt[j] in FOLLOW[new_vn[i]]:
                                if analysis_table[i + 1][j + 1] is None:
                                    analysis_table[i + 1][j + 1] = t
        # 判断是否为LL（1）文法
        is_ll1 = True
        for i in range(1, len(new_vn) + 1):
            for j in range(1, len(new_vt) + 1):
                if analysis_table[i][j] is not None and analysis_table[i][j] != 'ε':  # 如果当前位置有值且不是空串
                    for k in range(i + 1, len(new_vn) + 1):  # 对比同一非终结符的其他产生式
                        if analysis_table[k][j] == analysis_table[i][j] and analysis_table[k][j] != 'ε':  # 排除空串
                            is_ll1 = False
                            break
                if not is_ll1:
                    break
            if not is_ll1:
                break

        if is_ll1:
            print("\n\n该文法是LL(1)文法")
        else:
            print("\n\n该文法不是LL(1)文法")

        # 输出分析表
        pretty_table_title = ['非终结符']
        for i in new_vt:
            pretty_table_title.append(i)
        analysis_pretty_table = PrettyTable(pretty_table_title)
        for i in range(len(analysis_table) - 1):
            analysis_pretty_table.add_row(analysis_table[i + 1])
        print('\n\n预测分析表:\n', analysis_pretty_table)

        # 返回预处理结构
        # print("new_vn:",new_vn[0])
        return new_vt, new_vn, analysis_table, '$' + new_vn[0]

    def get_first_and_follow_set(self, grammars, vn, vt):
        FIRST = {}
        FOLLOW = {}
        index = 0

        # 初始化 first 和 follow 集合
        for str in grammars:
            part_begin = str.split("->")[0]
            part_end = str.split("->")[1]
            FIRST[part_begin] = ""
            FOLLOW[part_begin] = ""
            index += 1

        # 设置开始符号的 FOLLOW 集合为 $
        start_symbol = grammars[0].split("->")[0]  # 假设第一个产生式的左部是开始符号
        FOLLOW[start_symbol] = "$"

        # first 集
        # 1. 处理文法中的所有终结符
        vm = vt + vn  # 合并终结符和非终结符
        for rule in grammars:
            part_begin, part_end = rule.split("->")
            if part_end[0] != 'ε' and part_end[0] in vt:  # 如果第一个字符是终结符
                FIRST[part_begin] += part_end[0]  # 将第一个终结符加入到 FIRST 集合

        # 2. 处理文法中的非终结符，递归添加其 FIRST 集合
        changed = True
        while changed:
            changed = False
            for rule in grammars:
                part_begin, part_end = rule.split("->")
                # 处理 A -> B 形式
                can_add_epsilon = True  # 用来判断右边的所有符号是否都可以推出 ε
                for i in range(len(part_end)):
                    first_symbol = part_end[i]
                    # 如果是终结符，直接加入到 FIRST 集合
                    if first_symbol in vt:
                        if first_symbol not in FIRST[part_begin]:
                            FIRST[part_begin] += first_symbol
                            changed = True
                        can_add_epsilon = False  # 遇到终结符后，不能推导出 ε
                        break
                    # 如果是非终结符
                    elif first_symbol in vn:
                        # 如果该非终结符可以推出 ε，则继续添加其 FIRST 集合
                        for symbol in FIRST[first_symbol]:
                            if symbol != 'ε' and symbol not in FIRST[part_begin]:
                                FIRST[part_begin] += symbol
                                changed = True
                        # 如果非终结符无法推出 ε，跳出循环
                        if 'ε' not in FIRST[first_symbol]:
                            can_add_epsilon = False
                            break
                    # 如果遇到 ε，则继续处理
                    elif first_symbol == 'ε':
                        if 'ε' not in FIRST[part_begin]:
                            FIRST[part_begin] += 'ε'
                            changed = True
                # 如果右侧所有符号都可以推出 ε，则加入 ε 到 part_begin 的 FIRST 集
                if can_add_epsilon and 'ε' not in FIRST[part_begin]:
                    FIRST[part_begin] += 'ε'
                    changed = True

        # 去重，确保每个集合中的字符都是唯一的
        for non_terminal in FIRST:
            FIRST[non_terminal] = ''.join(sorted(set(FIRST[non_terminal])))
        # follow 集
        for i in range(len(vn)):
            while True:
                test = FOLLOW.copy()  # 使用 copy 来判断是否有变化
                for rule in grammars:
                    part_begin, part_end = rule.split("->")
                    # S -> a 直接推出终结符则继续
                    if (len(match_strings(vm, part_end)) == 1 and part_end in vt):
                        continue
                    else:
                        temp = match_strings(vm + ["ε"], reverse_by_set(vm + ["ε"], part_end))
                        if temp[0] in vn:
                            FOLLOW[temp[0]] = FOLLOW.get(temp[0], '') + FOLLOW.get(part_begin, '')
                            temp1 = temp[0]
                            for i in temp[1:]:
                                if i in vt:
                                    temp1 = i
                                else:
                                    if temp1 in vn:
                                        FOLLOW[i] = FOLLOW.get(i, '') + FIRST.get(temp1, '').replace("ε", "")
                                    first_set = FIRST.get(temp1, set())
                                    if 'ε' in first_set:
                                        FOLLOW[i] = FOLLOW.get(i, '') + FOLLOW.get(part_begin, '')
                                    temp1 = i
                        else:
                            temp1 = temp[0]
                            for i in temp[1:]:
                                if i in vt:
                                    temp1 = i
                                else:
                                    if temp1 in vn:
                                        FOLLOW[i] = FOLLOW.get(i, '') + FIRST.get(temp1, '')
                                    else:
                                        FOLLOW[i] = FOLLOW.get(i, '') + temp1
                                    temp1 = i
                # follow 集去重
                for i, j in FOLLOW.items():
                    FOLLOW[i] = ''.join(sorted(set(j)))

                # 去除 FOLLOW 集中的 'ε'
                for non_terminal in FOLLOW:
                    FOLLOW[non_terminal] = FOLLOW[non_terminal].replace('ε', '')

                if test == FOLLOW:
                    break

        return FIRST, FOLLOW

    # LL（1）分析过程
    def LL1_analysis_solve(self, goal_str, ans_table):
        vt, vn, analysis_table, stack_str, ptr = self.vt, self.vn, self.analysis_table, self.stack_str, self.ptr
        vm = vn + vt
        goal_str = match_strings(vm + ["ε"], goal_str)
        stack_str = match_strings(vm + ["ε"], stack_str)
        lookup_table = None
        shuchu = ''

        while ptr >= 0 and ptr < len(goal_str):  # 确保指针在输入字符串范围内
            if not stack_str:
                print("分析失败！栈为空，输入串未完全匹配。")
                return

            stack_top = stack_str[len(stack_str) - 1]  # 获取栈顶
            goal_pos = goal_str[ptr]  # 获取当前输入符号

            # 非法输入的情况
            if (stack_top not in vt and stack_top not in vn) or goal_pos not in vt:
                print('输入不合法！')
                return
            elif stack_top == goal_pos:  # 栈顶符号 = 当前输入符号
                if stack_top == '$':  # 栈顶符号 = 当前输入符号 = '$'
                    print('分析成功！')
                    ans_table.add_row([stack_str, goal_str[ptr:], '分析成功'])
                    return
                else:  # 栈顶符号 = 当前输入符号，但是不等于 $
                    ans_table.add_row([stack_str, goal_str[ptr:], ''])
                    stack_str = stack_str[0:len(stack_str) - 1]  # 弹栈
                    ptr += 1  # 输入指针前移
                    continue

            # 如果栈顶是非终结符
            if stack_top in vn:
                stack_top_index = vn.index(stack_top)
                goal_pos_index = vt.index(goal_pos)
                # 防止索引越界
                if stack_top_index < len(analysis_table) and goal_pos_index < len(analysis_table[0]):
                    lookup_table = analysis_table[stack_top_index + 1][goal_pos_index + 1]
                    if lookup_table is not None:  # 如果找到对应的产生式
                        # 弹栈处理 ε
                        if lookup_table == 'ε':
                            shuchu = "".join(stack_top) + "->" + "".join(lookup_table)
                            ans_table.add_row([stack_str, goal_str[ptr:], shuchu])
                            stack_str = stack_str[0:len(stack_str) - 1]  # 弹栈
                            continue
                        else:
                            # 存在对应产生式，反向压栈
                            shuchu = "".join(stack_top) + "->" + "".join(lookup_table)
                            ans_table.add_row([stack_str, goal_str[ptr:], shuchu])
                            stack_str = stack_str[0:len(stack_str) - 1]  # 弹栈
                            stack_str += match_strings(vm + ["ε"], reverse_by_set(vm + ["ε"], lookup_table))
                            continue
                    else:
                        print('分析失败！没有找到对应的产生式。')
                        return
                else:
                    print(f"分析失败！索引越界，stack_top_index: {stack_top_index}, goal_pos_index: {goal_pos_index}")
                    return

            # 如果栈顶是终结符，直接匹配
            elif stack_top in vt:
                if stack_top == goal_pos:  # 栈顶符号和输入符号匹配
                    stack_str = stack_str[0:len(stack_str) - 1]  # 弹栈
                    ptr += 1  # 输入指针前移
                    continue
                else:
                    print(f"分析失败！栈顶符号 {stack_top} 与输入符号 {goal_pos} 不匹配。")
                    return


if __name__ == '__main__':
    with open(r'D:\bianyiyuanli3\test', 'r', encoding='utf-8') as file:
        ll1_analysis = LL1_analysis(Gram=file.read())
    while True:
        goal_str = input('请输入字符串(end退出):') + '$'
        if goal_str == 'end$':
            break
        result_table = PrettyTable(['栈', '输入串', '寻找产生式'])
        ll1_analysis.LL1_analysis_solve(goal_str=goal_str, ans_table=result_table)
        print(result_table)
