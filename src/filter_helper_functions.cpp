#include "inc/filter_helper_functions.h"

bool substrFilterHandler(SubstringFilter *sf, int *err,
                         std::vector<unsigned char> attribute) {
  std::vector<unsigned char> attributeInital = {};
  std::vector<unsigned char> attributeMiddle = {};
  std::vector<unsigned char> attributeFinal = {};

  // extract initial
  if (!sf->getSubInitial().empty()) {
    if (attribute.size() >= sf->getSubInitial().size()) {
      attributeInital = std::vector<unsigned char>(
          attribute.begin(), attribute.begin() + sf->getSubInitial().size());
      attributeMiddle = std::vector<unsigned char>(
          attribute.begin() + sf->getSubInitial().size(), attribute.end());
    } else {
      return false;
    }
  } else {
    attributeMiddle =
        std::vector<unsigned char>(attribute.begin(), attribute.end());
  }

  // extract final
  if (!sf->getSubFinal().empty()) {
    if (attribute.size() >= sf->getSubFinal().size()) {
      attributeFinal = std::vector<unsigned char>(
          attribute.end() - sf->getSubFinal().size(), attribute.end());
      if (attributeMiddle.size() > sf->getSubFinal().size()) {
        attributeMiddle = std::vector<unsigned char>(
            attributeMiddle.begin(),
            attributeMiddle.end() - sf->getSubFinal().size());
      }
    } else {
      return false;
    }
  }

  // check subInitial
  if (!sf->getSubInitial().empty() &&
      ToLowerCase(attributeInital) != ToLowerCase(sf->getSubInitial())) {
    return false;
  }
  // check subFinal
  if (!sf->getSubFinal().empty() &&
      ToLowerCase(attributeFinal) != ToLowerCase(sf->getSubFinal())) {
    return false;
  }
  unsigned long int x = 0;

  // convert to lower case
  attributeMiddle = ToLowerCase(attributeMiddle);

  for (unsigned long int y = 0; y < sf->getSubAny().size(); y++) {
    bool match = false;
    for (; x < attributeMiddle.size(); x++) {

      if (attributeMiddle[x] == std::tolower(sf->getSubAny()[y][0])) {
        match = true;
        for (unsigned long int z = 0; z < sf->getSubAny()[y].size(); z++) {
          if (attributeMiddle[x + z] != std::tolower(sf->getSubAny()[y][z])) {
            match = false;
            break;
          }
        }
        if (match) {
          break;
        }
      }
    }
    if (!match) {
      return false;
    }
  }
  return true;
}

bool equalityMatchHandler(EqualityMatchFilter *emf, int *err,
                          std::vector<unsigned char> attribute) {
  if (ToLowerCase(emf->getAssertionValue()) == ToLowerCase(attribute)) {
    return true;
  }
  return false;
}

bool filterLine(FilterObject *f, int *err, DatabaseObject &databaseEntry) {

  // cn
  std::vector<unsigned char> cn = {'c', 'n'};
  // CommonName
  std::vector<unsigned char> CommonName = {'c', 'o', 'm', 'm', 'o',
                                           'n', 'n', 'a', 'm', 'e'};
  // email
  std::vector<unsigned char> email = {'e', 'm', 'a', 'i', 'l'};
  // email
  std::vector<unsigned char> mail = {'m', 'a', 'i', 'l'};
  // uid
  std::vector<unsigned char> uid = {'u', 'i', 'd'};
  // UserID
  std::vector<unsigned char> UserID = {'u', 's', 'e', 'r', 'i', 'd'};

  switch (f->getFilterType()) {
  case equalityMatch: {
    std::vector<unsigned char> attributeDescription =
        ToLowerCase(((EqualityMatchFilter *)f)->getAttributeDescription());
    if (attributeDescription == cn || attributeDescription == CommonName) {
      return equalityMatchHandler((EqualityMatchFilter *)f, err,
                                  databaseEntry.get_name());
    } else if (attributeDescription == email || attributeDescription == mail) {
      return equalityMatchHandler((EqualityMatchFilter *)f, err,
                                  databaseEntry.get_email());
    } else if (attributeDescription == uid || attributeDescription == UserID) {
      return equalityMatchHandler((EqualityMatchFilter *)f, err,
                                  databaseEntry.get_uid());
    } else {
      *err = 2;
      return false;
    }

  } break;
  case substrings: {
    std::vector<unsigned char> attributeDescription =
           ToLowerCase(((SubstringFilter *)f)->getAttributeDescription());
    if (attributeDescription == cn || attributeDescription == CommonName) {
      return substrFilterHandler((SubstringFilter *)f, err,
                                 databaseEntry.get_name());
    } else if (attributeDescription == email || attributeDescription == mail) {
      return substrFilterHandler((SubstringFilter *)f, err,
                                 databaseEntry.get_email());
    } else if (attributeDescription == uid || attributeDescription == UserID) {
      return substrFilterHandler((SubstringFilter *)f, err,
                                 databaseEntry.get_uid());
    } else {
      *err = 2;
      return false;
    }
  } break;
  case AND: {
    AndFilter *af = (AndFilter *)f;
    for (unsigned long int i = 0; i < af->filters.size(); i++) {
      if (!filterLine(af->filters[i], err, databaseEntry)) {
        return false;
      }
    }
    return true;

  } break;
  case OR: {
    OrFilter *of = (OrFilter *)f;
    for (unsigned long int i = 0; i < of->filters.size(); i++) {
      if (filterLine(of->filters[i], err, databaseEntry)) {
        return true;
      }
    }
    return false;
  } break;
  case NOT: {
    NotFilter *nf = (NotFilter *)f;
    bool result = !filterLine((nf->filter), err, databaseEntry);
    if (*err == 2)
      return false;
    return result;
  } break;
  default:
    return false;
  }
  return false;
}

std::vector<DatabaseObject> filterHandler(FilterObject *f, int *err,
                                          const char *dbLocation,
                                          int sizeLimit) {
  std::vector<DatabaseObject> resultDB;
  int dbErr = 0;
  DatabaseController db(dbLocation);
  int lineCounter = 0;
  while (true) {

    DatabaseObject obj = db.loadNextRow(&dbErr);

    if (dbErr != 0)
      break;

    if (filterLine(f, err, obj)) {
      resultDB.push_back(obj);
      lineCounter++;
    }

    if (lineCounter >= sizeLimit && sizeLimit != 0) {
      *err = 1;
      break;
    }
  }

  return resultDB;
}

FilterObject *
convertToFilterObject(std::vector<unsigned char>::iterator BERfilter,
                      std::vector<unsigned char>::iterator end) {

  FilterObject *f;
  int err;
  int lenght = 0;
  int ll = 0;
  std::vector<unsigned char> attributeDescription;
  std::vector<unsigned char> assertionValue;

  switch (getFilterType(BERfilter)) {
  case equalityMatch:

    GoIntoTag(BERfilter, &err, end);
    if (err != 0)
      return new FilterObject();
    lenght = GetLength(BERfilter + 1, &err, end);

    ll = GetLengthOfLength(BERfilter + 1, &err, end);

    for (int i = 0; i < lenght; i++) {
      attributeDescription.push_back(BERfilter[1 + ll + i]);
    }

    SkipTags(BERfilter, 1, &err, end);
    if (err != 0)
      return new FilterObject();

    lenght = GetLength(BERfilter + 1, &err, end);
    ll = GetLengthOfLength(BERfilter + 1, &err, end);

    for (int i = 0; i < lenght; i++) {
      assertionValue.push_back(BERfilter[1 + ll + i]);
    }

    f = new EqualityMatchFilter(attributeDescription, assertionValue);

    break;
  case substrings: {
    std::vector<unsigned char> attributeDescription;
    std::vector<unsigned char> initial;
    std::vector<std::vector<unsigned char>> any;
    std::vector<unsigned char> final;
    GoIntoTag(BERfilter, &err, end);
    if (err != 0)
      return new FilterObject();
    lenght = GetLength(BERfilter + 1, &err, end);
    ll = GetLengthOfLength(BERfilter + 1, &err, end);

    for (int i = 0; i < lenght; i++) {
      attributeDescription.push_back(BERfilter[1 + ll + i]);
    }

    SkipTags(BERfilter, 1, &err, end);
    if (err != 0)
      return new FilterObject();
    int lenghtOfSequence = GetLength(BERfilter + 1, &err, end);
    int lenghtOflenOfSequence = GetLengthOfLength(BERfilter + 1, &err, end);
    GoIntoTag(BERfilter, &err, end);
    if (err != 0)
      return new FilterObject();

    int currentBitPointer = 0;
    while (currentBitPointer < lenghtOfSequence + lenghtOflenOfSequence) {
      switch (BERfilter[0]) {
      case 0x80: {
        int dataLenght = GetLength(BERfilter + 1, &err, end);
        int LenghtLenght = GetLengthOfLength(BERfilter + 1, &err, end);
        initial = std::vector<unsigned char>(BERfilter + 1 + LenghtLenght,
                                             BERfilter + 1 + LenghtLenght +
                                                 dataLenght);
      } break;
      case 0x81: {
        int dataLenght = GetLength(BERfilter + 1, &err, end);
        int LenghtLenght = GetLengthOfLength(BERfilter + 1, &err, end);
        std::vector<unsigned char> tmp = std::vector<unsigned char>(
            BERfilter + 1 + LenghtLenght,
            BERfilter + 1 + LenghtLenght + dataLenght);
        any.push_back(tmp);
      } break;
      case 0x82: {
        int dataLenght = GetLength(BERfilter + 1, &err, end);
        int LenghtLenght = GetLengthOfLength(BERfilter + 1, &err, end);
        final = std::vector<unsigned char>(BERfilter + 1 + LenghtLenght,
                                           BERfilter + 1 + LenghtLenght +
                                               dataLenght);
      } break;
      }
      currentBitPointer += 1 + GetLengthOfLength(BERfilter + 1, &err, end) +
                           GetLength(BERfilter + 1, &err, end);
      SkipTags(BERfilter, 1, &err, end);
    }
    f = new SubstringFilter(attributeDescription, initial, any, final);

  } break;
  case AND:
    f = new AndFilter();

    lenght = GetLength(BERfilter + 1, &err, end);
    GoIntoTag(BERfilter, &err, end);
    if (err != 0)
      return new FilterObject();
    for (int i = 0; i < lenght;) {

      if (err != 0)
        return new FilterObject();
      FilterObject *tmpF = convertToFilterObject(BERfilter, end);
      printf("filter type: %d\n", tmpF->getFilterType());
      fflush(stdout);
      ((AndFilter *)f)->filters.push_back(tmpF);
      i += 1 + GetLengthOfLength(BERfilter + 1, &err, end) +
           GetLength(BERfilter + 1, &err, end);
      SkipTags(BERfilter, 1, &err, end);
    }

    break;
  case OR:
    f = new OrFilter();
    lenght = GetLength(BERfilter + 1, &err, end);
    GoIntoTag(BERfilter, &err, end);
    if (err != 0)
      return new FilterObject();

    for (int i = 0; i < lenght;) {
      if (err != 0)
        return new FilterObject();
      FilterObject *tmpF = convertToFilterObject(BERfilter, end);
      ((OrFilter *)f)->filters.push_back(tmpF);
      i += 1 + GetLengthOfLength(BERfilter + 1, &err, end) +
           GetLength(BERfilter + 1, &err, end);
      SkipTags(BERfilter, 1, &err, end);
    }

    break;
  case NOT: {
    f = new NotFilter();
    lenght = GetLength(BERfilter + 1, &err, end);
    GoIntoTag(BERfilter, &err, end);
    if (err != 0)
      return new FilterObject();
    FilterObject *tmpF = convertToFilterObject(BERfilter, end);
    ((NotFilter *)f)->filter = tmpF;
    break;
  }
  default:
    f = new FilterObject();
    break;
  }
  return f;
}