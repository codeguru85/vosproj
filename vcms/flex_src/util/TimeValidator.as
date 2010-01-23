package util
{
	import mx.validators.ValidationResult;
	import mx.validators.Validator;
	import mx.events.DataGridEvent;
	/**
	 * TimeValidator 验证时间格式是否正确
	 * */
	public class TimeValidator extends Validator
	{
		/**
		 * 时间格式的验证方法
		 * */
		[Bindable]
		public var isValidDate:Function;
		
		/**
		 * 错误提示信息
		 * */		
		[Bindable]
		private var _timeFormatError:String;
			
			
		public function TimeValidator():void{
			super();
		}
		
		public function set timeFormatError(value:String):void{
			this._timeFormatError = value;
		}
		
		/**
		 * 重写父类的验证方法
		 * */
		protected override function doValidation(value:Object):Array{
			var results:Array = [];
			var result:ValidationResult = validateTime(value.toString());
	        if (result)
	        	results.push(result);   
	              
	        return results;
        }
         
		protected function validateTime(value:String):ValidationResult{
			if(!isValidDate(value))
				return new ValidationResult(true, "", "requiredField",this._timeFormatError); 
		    return null;
		}         
	}
}