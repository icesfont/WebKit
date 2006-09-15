/*
    Copyright (C) 2004, 2005, 2006 Nikolas Zimmermann <zimmermann@kde.org>
                  2004, 2005 Rob Buis <buis@kde.org>

    This file is part of the KDE project

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#ifndef SVGList_H
#define SVGList_H

#ifdef SVG_SUPPORT

#include <wtf/Vector.h>

#include "Shared.h"
#include "SVGListTraits.h"
#include "ExceptionCode.h"

namespace WebCore {

    template<typename Item>
    struct SVGListTypeOperations
    {
        static Item nullItem()
        {
            return SVGListTraits<UsesDefaultInitializer<Item>::value, Item>::nullItem();
        }
    };

    template<typename Item>
    class SVGList : public Shared<SVGList<Item> >
    {
    private:
        typedef SVGListTypeOperations<Item> TypeOperations;

    public:
        SVGList() { }
        virtual ~SVGList() { m_vector.clear(); }

        unsigned int numberOfItems() const { return m_vector.size(); }
        void clear(ExceptionCode &) { m_vector.clear(); }

        Item initialize(Item newItem, ExceptionCode& ec)
        {
            clear(ec);
            return appendItem(newItem, ec);
        }

        Item getFirst() const
        {
            if (m_vector.isEmpty())
                return TypeOperations::nullItem();

            return m_vector.first();
        }

        Item getLast() const
        {
            if (m_vector.isEmpty())
                return TypeOperations::nullItem();

            return m_vector.last();
        }

        Item getItem(unsigned int index, ExceptionCode& ec)
        {
            if (m_vector.size() < index) {
                ec = INDEX_SIZE_ERR;
                return TypeOperations::nullItem();
            }

            return m_vector.at(index);
        }

        const Item getItem(unsigned int index, ExceptionCode& ec) const
        {
            if (m_vector.size() < index) {
                ec = INDEX_SIZE_ERR;
                return TypeOperations::nullItem();
            }

            return m_vector[index];
        }

        Item insertItemBefore(Item newItem, unsigned int index, ExceptionCode&)
        {
            m_vector.insert(index, newItem);
            return newItem;
        }

        Item replaceItem(Item newItem, unsigned int index, ExceptionCode& ec)
        {
            if (m_vector.size() < index) {
                ec = INDEX_SIZE_ERR;
                return TypeOperations::nullItem();
            }

            m_vector[index] = newItem;
            return newItem;
        }

        Item removeItem(unsigned int index, ExceptionCode& ec)
        {
            if (m_vector.size() < index) {
                ec = INDEX_SIZE_ERR;
                return TypeOperations::nullItem();
            }

            Item item = m_vector[index];
            removeItem(index, ec);
            return item;
        }

        void removeItem(const Item item)
        {
            m_vector.remove(item);
        }

        Item appendItem(Item newItem, ExceptionCode&)
        {
            m_vector.append(newItem);
            return newItem;
        }

    private:
        Vector<Item> m_vector;
    };

} // namespace WebCore

#endif // SVG_SUPPORT
#endif // SVGList_H

// vim:ts=4:noet
