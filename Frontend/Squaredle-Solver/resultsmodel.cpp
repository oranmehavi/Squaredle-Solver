#include "resultsmodel.h"

ResultsModel::ResultsModel(QObject *parent)
    : QAbstractListModel(parent),
    m_totalWordsCount(0)
{}

int ResultsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_results.count();
}

bool ResultsModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(QModelIndex(), row, row + count - 1);
    m_results.clear();
    endRemoveRows();
    return true;
}

QVariant ResultsModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_results.count())
    {
        return QVariant();
    }

    Words result = m_results[index.row()];
    if (role == WordsRole)
    {
        return result.words;
    }
    else if (role == LengthRole)
    {
        return result.length;
    }
    else if (role == SectionCountRole)
    {
        return result.words.count();
    }
    return QVariant();
}

QList<Words> ResultsModel::results() const
{
    return m_results;
}

void ResultsModel::setResults(const QList<Words> &newResults)
{
    m_results = newResults;
}

void ResultsModel::createResults(std::vector<std::string> &found_words)
{
    if (m_results.count() > 0)
    {
        erasePreviousResults();
    }

    m_totalWordsCount = found_words.size();
    emit totalWordsCountChanged();

    found_words.push_back(""); // makes sure the longest length words will be shown
    QStringList temp;
    size_t previous_word_length = found_words[0].length();
    for (size_t i = 0; i < found_words.size(); ++i)
    {
        if (found_words[i].length() != previous_word_length)
        {
            beginInsertRows(QModelIndex(), rowCount(), rowCount());
            m_results << Words{static_cast<int>(previous_word_length), temp};
            endInsertRows();
            temp.clear();
            previous_word_length = found_words[i].length();
        }

        temp << QString::fromStdString(found_words[i]);
    }
}

QHash<int, QByteArray> ResultsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[WordsRole] = "words";
    roles[LengthRole] = "length";
    roles[SectionCountRole] = "section_count";
    return roles;
}

void ResultsModel::erasePreviousResults()
{
    m_totalWordsCount = 0;
    emit totalWordsCountChanged();

    removeRows(0, rowCount(), index(0,0));
}

int ResultsModel::totalWordsCount() const
{
    return m_totalWordsCount;
}
