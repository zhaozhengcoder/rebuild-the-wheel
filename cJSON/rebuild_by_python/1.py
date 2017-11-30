
class cJSON(object):
    def __init__(self):
        self.next=None
        self.prev=None
        self.child=None
        self.type=None
        self.valuestring=None  #value
        #self.valueint=None
        self.valuedouble=None
        self.string=None       #key

def parse_error():
    print ("parser_error !")
    exit(-1)

def skip(value):
    index=0
    for i in value:
        #print (ord(i))
        if(ord(i)<32):
           index=+1
        else:
            break
    return value[index:] 


def parse_number(c,value):
    index=0
    sign=1
    num=None
    if(value[0]=='-'):
        sign=-1
    for i in value[1:]:
        if((i>='0' and i<='9') or (i=='.')):
            index+=1
        else:
            break
    if(sign==-1):
        num=float(value[1:index+1])*(-1)
    else:
        num=float(value[:index+1])
    c.valuedouble=num
    c.type="cJSON_Number"
    print ("num: ",num)
    print ("ret :",value[index+1:])
    return value[index+1:]

def parse_string(c,value):
    if value[0]!='"':
        parse_error()
    index=0
    for i in value[1:]:
        if(i!='"'):
            index=index+1
        else:
            break
    parser_str=value[1:index+1]
    ret=value[index+2:]
    print ("str : ",parser_str)
    print ("ret : ",ret)
    c.valuestring=parser_str
    c.type="cJSON_String"
    return ret
        
def parse_object(item,value):
    if value[0]!="{":
        parse_error()
    else:
        value=value[1:]

    #空的object
    if(value[0]=="}"):
        return value[1:]
    
    child=cJSON()
    item.type="cJSON_Object"
    item.child=child

    value=skip(parse_string(child,skip(value)))
    child.string=child.valuestring

    #这里可能的情况是 {"name":"zhao"}
    #parse_string 函数，提取走了引号里面的值，即name，返回之后的value是 :"zhao"}
    #取消json的key-value中间的冒号
    if value[0]==':':
        value=value[1:]
    value=skip(parse_value(child,skip(value)))

    if (len(value)==0):
        return 0

    while(value[0]==','):
        new_item=cJSON()
        child.next=new_item
        new_item.prev=child
        child=new_item

        value=skip(parse_string(child,skip(value[1:])))
        if (len(value)==0):
            return 0
        child.string=child.valuestring
        child.valuestring=0

        if(value[0]==':'):
            value=value[1:]
        value=skip(parse_value(child,skip(value[1:])))
        if (len(value)==0):
            return 0

    if(value[0]=='}'):
        return value[1:]



def parse_value(c,value):
    if(value[0]=='['):
        print("array")
    elif(value[0]=='{'):
        print("object")
        return parse_object(c,value)
    elif(value[0]=="-" or (value[0]>='0' and value[0]<='9' )):
        print("number")
        return parse_number(c,value)
    elif(value[0]=='\"'):
        print("string")
        return parse_string(c,value)
    else:
        print ("other")


def pretty_print(c):
    currnet=c
    if(currnet.type=="cJSON_Object"):
        print ("{")
        pretty_print(currnet.child)
        print ("}")
    if(currnet.type=="cJSON_String"):
        print (' "{}":"{}"'.format(currnet.string,currnet.valuestring))
        while(currnet.next!=None):
            currnet=currnet.next
            print (",")
            if(currnet.type=="cJSON_String"):
                print (' "{}":"{}"'.format(currnet.string,currnet.valuestring))
            if(currnet.type=="cJSON_Number"):
                print (' "{}":{}'.format(currnet.string,currnet.valuedouble))



def cJSON_ParseWithOpts(value):
    c=cJSON()
    parse_value(c,skip(value))
    print ("end")
    print (c)
    pretty_print(c)
    print ("show json")
    




if __name__=="__main__":
    text='{"name":"zhao","edu":{"benke":"ujs","shuoshi":"tju"}}'
    #text="-123.45,{name:zhao}"
    cJSON_ParseWithOpts(text)
