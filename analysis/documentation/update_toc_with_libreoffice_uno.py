from pathlib import Path
import sys
import uno
from com.sun.star.beans import PropertyValue

DOC = Path('/home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/documentation/DynamicPackage_单星仿真_基础原理_逐式符号注释_最终版_目录已刷新_H0-H21_10万步验证.docx').resolve()


def prop(name, value):
    p = PropertyValue()
    p.Name = name
    p.Value = value
    return p


def main():
    local_ctx = uno.getComponentContext()
    resolver = local_ctx.ServiceManager.createInstanceWithContext('com.sun.star.bridge.UnoUrlResolver', local_ctx)
    ctx = resolver.resolve('uno:socket,host=127.0.0.1,port=2002;urp;StarOffice.ComponentContext')
    desktop = ctx.ServiceManager.createInstanceWithContext('com.sun.star.frame.Desktop', ctx)
    doc = desktop.loadComponentFromURL(uno.systemPathToFileUrl(str(DOC)), '_blank', 0, (prop('Hidden', True),))
    if doc is None:
        raise RuntimeError('LibreOffice 未能打开 DOCX')
    try:
        # 目录域在 DOCX 导入后会表现为文本域或目录索引；两类均刷新。
        text_fields = doc.getTextFields()
        text_fields.refresh()
        indexes = doc.getDocumentIndexes()
        count = indexes.getCount()
        for i in range(count):
            indexes.getByIndex(i).update()
        doc.updateLinks()
        doc.store()
        print(f'document_indexes_updated={count}')
        print('saved=true')
    finally:
        doc.close(True)


if __name__ == '__main__':
    main()
