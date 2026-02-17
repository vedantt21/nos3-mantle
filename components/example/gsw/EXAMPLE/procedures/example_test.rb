require 'cosmos'
require 'cosmos/script'
require 'example_lib.rb'

class EXAMPLE_Functional_Test < Cosmos::Test
  def setup
    safe_example()
  end

  def test_application
      start("tests/example_app_test.rb")
  end

  def test_device
    start("tests/example_device_test.rb")
  end

  def teardown
    safe_example()
  end
end

class EXAMPLE_Automated_Scenario_Test < Cosmos::Test
  def setup 
    safe_example()
  end

  def test_AST
      start("tests/example_ast_test.rb")
  end

  def teardown
    safe_example()
  end
end

class Example_Test < Cosmos::TestSuite
  def initialize
      super()
      add_test('EXAMPLE_Functional_Test')
      add_test('EXAMPLE_Automated_Scenario_Test')
  end

  def setup
    safe_example()
  end
  
  def teardown
    safe_example()
  end
end
