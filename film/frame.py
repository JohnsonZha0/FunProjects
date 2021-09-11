import csv
import tkinter
from tkinter import *
import os
import matplotlib.pyplot as plt
from tkinter import ttk
import functools
from matplotlib import font_manager as fm, rcParams


# 将txt格式转化成csv格式的文件存储
def txt_to_csv():
    with open('电影资料.csv', 'w', encoding='utf8') as wf:
        writer = csv.writer(wf)
        with open('电影资料(2010-2020).txt', 'r', encoding='utf8') as f:
            for line in f:
                # 根据每行按逗号和中括号去提取各个字段
                line = line.replace('\n', '')
                name = line[:line.find(',')]
                line = line[line.find(',') + 1:]
                score = line[:line.find(',')]
                line = line[line.find(',') + 1:]
                types = line[line.find('[') + 1:line.find(']') - 1].replace("'", '').replace('"', '')
                line = line[line.find(']') + 2:]
                date = line[:line.find(',')]
                actors = line[line.find(',') + 1:].replace('[', '').replace(']', '').replace("'", '').replace('"', '')
                row = [name, score] + types.split(',') + [date] + actors.split(',')
                print(row)
                writer.writerow(row)


def load_films():
    with open('电影资料.csv', 'r', encoding='utf8') as f:
        reader = csv.reader(f)
        data = []
        type_index = {}  # 按类型建立索引  即 类型和电影的映射关系
        actor_index = {}  # 按演员建立索引 即 演员和电影的映射关系
        year_index = {}  # 按年份建立索引 即 年份和电影的映射关系
        # 按演员建立索引 即 演员和电影的映射关系
        for row in reader:
            if len(row) == 0:
                continue
            film = {}
            film['name'] = row[0]
            film['score'] = row[1]
            film['types'] = []
            index = 2
            for i in range(2, len(row)):
                elem = row[i]
                if elem.isdigit():
                    index = i
                    break
                film['types'].append(elem)
                if elem not in type_index:
                    type_index[elem] = []
                type_index[elem].append(film)
            film['publish_time'] = row[index]
            if row[index] not in year_index:
                year_index[row[index]] = []
            year_index[row[index]].append(film)
            film['actors'] = []
            for i in range(index + 1, len(row)):
                elem = row[i]
                film['actors'].append(elem)
                if elem not in actor_index:
                    actor_index[elem] = []
                actor_index[elem].append(film)
            data.append(film)
        return data, type_index, actor_index, year_index

# 将电影的格式转换成适用表格的形式
def film_to_row(film):
    row = []
    row.append(film['name'])
    row.append(film['score'])
    row.append(' '.join(film['types']))
    row.append(film['publish_time'])
    row.append(' '.join(film['actors']))
    return row


def show_histogram_by_year():
    years = YEAR_INDEX.keys()
    years = sorted(years)
    year_value = []  # 年份对应的电影数
    for y in years:
        year_value.append(len(YEAR_INDEX[y]))
    plt.figure(figsize=(6, 3))
    plt.rcParams['font.sans-serif'] = ['SimHei']  # 显示中文标签
    plt.rcParams['axes.unicode_minus'] = False
    plt.bar(years, year_value, label="年份电影数量分布图")
    plt.xticks(rotation=-15)
    plt.show()


def show_histogram_by_type():
    types = TYPE_INDEX.keys()
    type_value = []
    for t in types:
        type_value.append(len(TYPE_INDEX[t]))
    plt.figure(figsize=(17, 3))
    plt.rcParams['font.sans-serif'] = ['SimHei']  # 显示中文标签
    plt.rcParams['axes.unicode_minus'] = False
    plt.bar(types, type_value, label="电影类型数量分布图")
    plt.xticks(rotation=-45)
    plt.show()

# 清除表格里的数据
def clear_table(tree):
    x = tree.get_children()
    for item in x:
        tree.delete(item)


def update_table(films, table):
    clear_table(table)
    idx = 0
    for film in films:
        row = film_to_row(film)
        table.insert('', idx, idx, values=row)
        idx += 1


current_films = None


def init_frame():
    frame = tkinter.Tk()
    Button(frame, text='年份柱状图', anchor='c', command=show_histogram_by_year).grid(row=2, column=0)
    Button(frame, text='类型柱状图', anchor='c', command=show_histogram_by_type).grid(row=2, column=1)
    search_text = tkinter.StringVar()
    search_text.set('')
    tkinter.Entry(frame, width=20, textvariable=search_text).grid(row=2, column=2)

    columns = ["名称", "评分", "类型", "年份", "演员"]
    table = ttk.Treeview(frame, columns=columns, height=40, show="headings")

    def search():
        global current_films
        type = search_text.get()
        if type in TYPE_INDEX:
            films = TYPE_INDEX[type]
            update_table(films, table)
            current_films = films

    def sort_by_year():
        global current_films

        def cmp(x, y):
            if x['publish_time'] > y['publish_time']:
                return -1
            elif x['publish_time'] < y['publish_time']:
                return 1
            else:
                return 0

        current_films = sorted(current_films, key=functools.cmp_to_key(cmp))
        update_table(current_films, table)

    Button(frame, text='搜索', anchor='c', command=search).grid(
        row=2, column=3)
    Button(frame, text='年份排序', anchor='c', command=sort_by_year).grid(
        row=2, column=4)
    table.column("名称", width=100, anchor="sw")
    table.column("评分", width=50, anchor="sw")
    table.column("类型", width=200, anchor="sw")
    table.column("年份", width=50, anchor="sw")
    table.column("演员", width=900, anchor="sw")
    table.heading("名称", text="名称")
    table.heading("评分", text="评分")
    table.heading("类型", text="类型")
    table.heading("年份", text="年份")
    table.heading("演员", text="演员")
    table.grid(row=3, column=0, columnspan=5)

    vbar = ttk.Scrollbar(frame, orient=VERTICAL, command=table.yview)
    table.configure(yscrollcommand=vbar.set)
    vbar.grid(row=3, column=5, sticky=NS)

    hbar = ttk.Scrollbar(frame, orient=HORIZONTAL, command=table.xview)
    table.configure(xscrollcommand=hbar.set)
    hbar.grid(row=4, column=0, sticky=EW, columnspan=5)

    frame.mainloop()

# 初始化基础数据
if not os.path.exists('电影资料.csv'):
    txt_to_csv()
FILMS, TYPE_INDEX, ACTOR_INDEX, YEAR_INDEX = load_films()
print(FILMS[0])

def main():
    init_frame()

if __name__ == '__main__':
    main()
